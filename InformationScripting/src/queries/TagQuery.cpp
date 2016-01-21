/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "TagQuery.h"

#include "../query_framework/QueryRegistry.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/util/SymbolMatcher.h"

#include "../nodes/TagExtension.h"

namespace InformationScripting {

const QStringList TagQuery::NAME_ARGUMENT_NAMES{"n", "name"};
const QStringList TagQuery::ADD_ARGUMENT_NAMES{"a", "add"};
const QStringList TagQuery::REMOVE_ARGUMENT_NAMES{"r", "remove"};
const QStringList TagQuery::PERSISTENT_ARGUMENT_NAMES{"p", "persistent"};

Optional<TupleSet> TagQuery::executeLinear(TupleSet input)
{
	return exec_(this, input);
}

void TagQuery::registerDefaultQueries()
{
	QueryRegistry::registerQuery<TagQuery>("tags", &TagQuery::tags,
		std::vector<ArgumentRule>{{ArgumentRule::AtMostOneOf, {{ADD_ARGUMENT_NAMES[1], ArgumentValue::IsSet},
												{REMOVE_ARGUMENT_NAMES[1], ArgumentValue::IsSet}}},
		 {ArgumentRule::RequireAll, {{NAME_ARGUMENT_NAMES[1]}}}});
	QueryRegistry::registerAlias("addTags", "tags", [](QStringList& args) {args.append("-" + ADD_ARGUMENT_NAMES[0]);});
	QueryRegistry::registerAlias("removeTags", "tags",
										  [](QStringList& args) {args.append("-" + REMOVE_ARGUMENT_NAMES[0]);});
}

TagQuery::TagQuery(Model::Node* target, QStringList args, ExecuteFunction exec, std::vector<ArgumentRule> argumentRules)
	: LinearQuery{target}, arguments_{{
			{NAME_ARGUMENT_NAMES, "Tag name, or regex to find tag", NAME_ARGUMENT_NAMES[1]},
			QCommandLineOption{ADD_ARGUMENT_NAMES},
			QCommandLineOption{REMOVE_ARGUMENT_NAMES},
			{PERSISTENT_ARGUMENT_NAMES, "Wether the change is persistent, default = yes", PERSISTENT_ARGUMENT_NAMES[1], "yes"}
}, args, true}, exec_{exec}
{
	persistent_ = arguments_.argument(PERSISTENT_ARGUMENT_NAMES[1]) == "yes";
	for (const auto& rule : argumentRules)
		rule.check(arguments_);
}

Optional<TupleSet> TagQuery::tags(TupleSet input)
{
	bool addSet = arguments_.isArgumentSet(ADD_ARGUMENT_NAMES[0]);
	bool removeSet = arguments_.isArgumentSet(REMOVE_ARGUMENT_NAMES[0]);
	Q_ASSERT(!(addSet && removeSet));
	if (addSet)
		return addTags(input);
	else if (removeSet)
		return removeTags(input).value();
	else
		return queryTags(input);
}

Optional<TupleSet> TagQuery::queryTags(TupleSet input)
{
	QString tagText = arguments_.argument(NAME_ARGUMENT_NAMES[0]);
	Q_ASSERT(tagText.size() > 0);
	// Keep stuff in the input
	TupleSet result = input;

	// Querying tags non persistent would just return the tag tuples in the input.
	if (!persistent_) return input;

	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local || scope == ArgumentParser::Scope::Global)
	{
		auto targetNode = scope == ArgumentParser::Scope::Local ? target() : nullptr;
		insertFoundTags(result, Model::SymbolMatcher::guessMatcher(tagText), targetNode);
	}
	else if (scope == ArgumentParser::Scope::Input)
	{
		auto matcher = Model::SymbolMatcher::guessMatcher(tagText);
		auto astTuples = input.tuples("ast");
		for (auto tuple : astTuples)
			insertFoundTags(result, matcher, tuple["ast"]);
	}
	return result;
}

Optional<TupleSet> TagQuery::addTags(TupleSet input)
{
	// Keep stuff in the input
	TupleSet result = input;
	QList<Model::Node*> addTagsTo;

	QString tagText = arguments_.argument(NAME_ARGUMENT_NAMES[0]);
	Q_ASSERT(tagText.size() > 0);

	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
		addTagsTo << target();
	else if (scope == ArgumentParser::Scope::Global)
	{
		// That doesn't make sense, to which nodes should we add the tags?
		// TODO: don't allow global argument!
	}
	else if (scope == ArgumentParser::Scope::Input)
		for (const auto& tuple : input.tuples("ast"))
			addTagsTo << static_cast<Model::Node*>(tuple["ast"]);

	if (persistent_)
	{
		auto treeManager = target()->manager();
		Q_ASSERT(treeManager);
		treeManager->beginModification(target(), "addTags");
		for (auto node : addTagsTo)
		{
			if (auto astNode = DCast<Model::CompositeNode>(node))
			{
				auto tagExtension = astNode->extension<TagExtension>();
				treeManager->changeModificationTarget(astNode);
				if (!tagExtension->tags())
					tagExtension->setTags(new Model::TypedList<Model::Text>{});
				tagExtension->tags()->append(new Model::Text{tagText});
			}
		}
		treeManager->endModification();
	}

	for (auto node : addTagsTo)
		result.add({{"tag", tagText}, {"ast", node}});

	return result;
}

Optional<TupleSet> TagQuery::removeTags(TupleSet input)
{
	QString tagText = arguments_.argument(NAME_ARGUMENT_NAMES[0]);
	Q_ASSERT(tagText.size() > 0);

	// Keep stuff in the input
	TupleSet result = input;
	auto matcher = Model::SymbolMatcher::guessMatcher(tagText);
	QString tagName = "tag";
	TupleSet removedTuples;
	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
		insertFoundTags(removedTuples, matcher, target());
	else if (scope == ArgumentParser::Scope::Global)
		insertFoundTags(removedTuples, matcher);
	else if (scope == ArgumentParser::Scope::Input)
	{
		auto astTuples = input.tuples("ast");
		for (auto tuple : astTuples)
			insertFoundTags(removedTuples, matcher, tuple["ast"]);
	}

	if (persistent_)
	{
		auto treeManager = target()->manager();
		Q_ASSERT(treeManager);
		treeManager->beginModification(target(), "removeTags");
		for (const auto& tuple : removedTuples.tuples(tagName))
		{
			Model::Node* node = tuple["ast"];
			if (auto astNode = DCast<Model::CompositeNode>(node))
			{
				auto tagExtension = astNode->extension<TagExtension>();
				auto tagsList = tagExtension->tags();
				if (tagsList)
				{
					treeManager->changeModificationTarget(tagsList);
					auto tagsIndex = 0;
					while (tagsIndex < tagsList->size())
					{
						if (matcher.matches(tagsList->at(tagsIndex)->get())) tagsList->remove(tagsIndex);
						else ++tagsIndex;
					}
				}
			}
		}
		treeManager->endModification();
	}

	for (const auto& tuple : removedTuples.take(tagName))
		result.remove(tuple);

	return result;
}

void TagQuery::insertFoundTags(TupleSet& tuples, const Model::SymbolMatcher& matcher, Model::Node* from)
{
	Q_ASSERT(persistent_);

	if (!from) from = target()->root();

	QList<Model::Node*> workStack{from};

	while (!workStack.empty())
	{
		auto node = workStack.takeLast();
		if (auto astNode = DCast<Model::CompositeNode>(node))
		{
			auto tagExtension = astNode->extension<TagExtension>();
			auto tags = tagExtension->tags();
			if (tags)
				for (auto tag : *tags)
					if (matcher.matches(tag->get()))
						tuples.add({{"tag", tag->get()}, {"ast", astNode}});
		}
		workStack << node->children();
	}
}

}
