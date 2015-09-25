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

#include "QueryRegistry.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/model/TreeManager.h"

#include "../nodes/TagExtension.h"

namespace InformationScripting {

const QStringList TagQuery::NAME_ARGUMENT_NAMES{"n", "name"};
const QStringList TagQuery::ADD_ARGUMENT_NAMES{"a", "add"};
const QStringList TagQuery::REMOVE_ARGUMENT_NAMES{"r", "remove"};
const QStringList TagQuery::PERSISTENT_ARGUMENT_NAMES{"p", "persistent"};

QList<TupleSet> TagQuery::execute(QList<TupleSet> input)
{
	return exec_(this, input);
}

void TagQuery::registerDefaultQueries()
{
	QueryRegistry::instance().registerQueryConstructor("tags", [](Model::Node* target, QStringList args) {
		return new TagQuery(&TagQuery::tags, target, args);
	});
	// Alias to "tags -a"
	QueryRegistry::instance().registerQueryConstructor("addTags", [](Model::Node* target, QStringList args) {
		return new TagQuery(&TagQuery::addTags, target, args);
	});
	// Alias to "tags -r"
	QueryRegistry::instance().registerQueryConstructor("removeTags", [](Model::Node* target, QStringList args) {
		return new TagQuery(&TagQuery::removeTags, target, args);
	});
}

TagQuery::TagQuery(ExecuteFunction<TagQuery> exec, Model::Node* target, QStringList args)
	: ScopedArgumentQuery{target, {
			{NAME_ARGUMENT_NAMES, "Tag name, or regex to find tag", NAME_ARGUMENT_NAMES[1]},
			QCommandLineOption{ADD_ARGUMENT_NAMES},
			QCommandLineOption{REMOVE_ARGUMENT_NAMES},
			{PERSISTENT_ARGUMENT_NAMES, "Wether the change is persistent, default = yes", PERSISTENT_ARGUMENT_NAMES[1], "yes"}
}, QStringList("TagQuery") + args}, exec_{exec}
{
	persistent_ = argument(PERSISTENT_ARGUMENT_NAMES[1]) == "yes";
}

QList<TupleSet> TagQuery::tags(QList<TupleSet> input)
{
	bool addSet = isArgumentSet(ADD_ARGUMENT_NAMES[0]);
	bool removeSet = isArgumentSet(REMOVE_ARGUMENT_NAMES[0]);
	Q_ASSERT(!(addSet && removeSet)); // TODO should be user warning
	if (addSet)
		return addTags(input);
	else if (removeSet)
		return removeTags(input);
	else
		return queryTags(input);
}

QList<TupleSet> TagQuery::queryTags(QList<TupleSet> input)
{
	QString tagText = argument(NAME_ARGUMENT_NAMES[0]);
	Q_ASSERT(tagText.size() > 0); // TODO should be user warning
	QList<TupleSet> result = input;

	// Querying tags non persistent would just return the tag tuples in the input.
	if (!persistent_) return input;

	if (scope() == Scope::Local || scope() == Scope::Global)
	{
		auto targetNode = scope() == Scope::Local ? target() : nullptr;
		if (result.empty()) result << TupleSet();
		insertFoundTags(result[0], matcherFor(tagText), targetNode);
	}
	else if (scope() == Scope::Input)
	{
		Q_ASSERT(input.size() > 0);

		auto matcher = matcherFor(tagText);
		auto astTuples = result[0].tuples("ast");
		for (auto tuple : astTuples)
			insertFoundTags(result[0], matcher, tuple["ast"]);
	}
	return result;
}

QList<TupleSet> TagQuery::addTags(QList<TupleSet> input)
{
	QList<TupleSet> result = input;
	QList<Model::Node*> addTagsTo;

	QString tagText = argument(NAME_ARGUMENT_NAMES[0]);
	Q_ASSERT(tagText.size() > 0); // TODO should be user warning

	if (scope() == Scope::Local)
	{
		if (result.empty()) result << TupleSet();
		addTagsTo << target();
	}
	else if (scope() == Scope::Global)
	{
		// That doesn't make sense, to which nodes should we add the tags?
		// TODO: warn user
	}
	else if (scope() == Scope::Input)
	{
		Q_ASSERT(input.size() > 0);
		for (const auto& tuple : result[0].tuples("ast"))
			addTagsTo << static_cast<Model::Node*>(tuple["ast"]);
	}

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
				tagExtension->tags()->append(new Model::Text{tagText});
			}
		}
		treeManager->endModification();
	}

	for (auto node : addTagsTo)
		result[0].add({{"tag", tagText}, {"ast", node}});

	return result;
}

QList<TupleSet> TagQuery::removeTags(QList<TupleSet> input)
{
	QString tagText = argument(NAME_ARGUMENT_NAMES[0]);
	Q_ASSERT(tagText.size() > 0); // TODO should be user warning

	QList<TupleSet> result = input;
	auto matcher = matcherFor(tagText);
	QString tagName = "tag";
	TupleSet removedTuples;
	if (scope() == Scope::Local)
		insertFoundTags(removedTuples, matcher, target());
	else if (scope() == Scope::Global)
		insertFoundTags(removedTuples, matcher);
	else if (scope() == Scope::Input)
	{
		Q_ASSERT(input.size() > 0);
		auto astTuples = result[0].tuples("ast");
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
				treeManager->changeModificationTarget(tagsList);
				auto tagsIndex = 0;
				while (tagsIndex < tagsList->size())
				{
					if (matcher.matches(tagsList->at(tagsIndex)->get())) tagsList->remove(tagsIndex);
					else ++tagsIndex;
				}
			}
		}
		treeManager->endModification();
	}

	if (result.size() > 0)
	{
		for (const auto& tuple : removedTuples.take(tagName))
			result[0].remove(tuple);
	}

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
			for (auto tag : *(tagExtension->tags()))
				if (matcher.matches(tag->get()))
					tuples.add({{"tag", tag->get()}, {"ast", astNode}});
		}
		workStack << node->children();
	}
}

} /* namespace InformationScripting */
