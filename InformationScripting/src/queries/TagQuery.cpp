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

const QStringList TagQuery::TAGTYPE_ARGUMENT_NAMES{"t", "type"};
const QStringList TagQuery::NAME_ARGUMENT_NAMES{"n", "name"};

QList<TupleSet> TagQuery::execute(QList<TupleSet> input)
{
	return exec_(this, input);
}

void TagQuery::registerDefaultQueries()
{
	QueryRegistry::instance().registerQueryConstructor("tags", [](Model::Node* target, QStringList args) {
		return new TagQuery(&TagQuery::queryTags, target, args);
	});
	QueryRegistry::instance().registerQueryConstructor("addTags", [](Model::Node* target, QStringList args) {
		return new TagQuery(&TagQuery::addTags, target, args);
	});
}

TagQuery::TagQuery(ExecuteFunction<TagQuery> exec, Model::Node* target, QStringList args)
	: ScopedArgumentQuery{target, {
			{TAGTYPE_ARGUMENT_NAMES, "Tag Type argument", TAGTYPE_ARGUMENT_NAMES[1]},
			{NAME_ARGUMENT_NAMES, "Name of a symbol", NAME_ARGUMENT_NAMES[1]},
		}, QStringList("TagQuery") + args}, exec_{exec}
{}

QList<TupleSet> TagQuery::queryTags(QList<TupleSet> input)
{
	QList<TupleSet> result;
	QList<Model::Text*> foundTags;
	if (scope() == Scope::Local)
		foundTags = allTags(target());
	else if (scope() == Scope::Global)
		foundTags = allTags();
	else if (scope() == Scope::Input)
	{
		Q_ASSERT(input.size() > 0);
		TupleSet tupleSet = input.takeFirst();
		auto astTuples = tupleSet.tuples("ast");
		for (auto tuple : astTuples)
		{
			Model::Node* node = tuple["ast"];
			if (auto astNode = DCast<Model::CompositeNode>(node))
			{
				auto tagExtension = astNode->extension<TagExtension>();
				for (auto tag : *tagExtension->tags())
					foundTags << tag;
			}
		}
		result << tupleSet;
	}

	for (auto tagText : foundTags)
		qDebug() << tagText->get();
	return result;
}

QList<TupleSet> TagQuery::addTags(QList<TupleSet> input)
{
	QList<TupleSet> result;
	QList<Model::Node*> addTagsTo;
	if (scope() == Scope::Local)
	{
		// Just add a tag to the target:
		addTagsTo << target();
	}
	else if (scope() == Scope::Global)
	{
		// TODO: does that make sense, to which nodes should we add the tags?
	}
	else if (scope() == Scope::Input)
	{
		Q_ASSERT(input.size() > 0);
		TupleSet tupleSet = input.takeFirst();
		auto astTuples = tupleSet.tuples("ast");

		for (auto tuple : astTuples)
			addTagsTo << static_cast<Model::Node*>(tuple["ast"]);

		result << tupleSet;
	}
	auto treeManager = target()->manager();
	treeManager->beginModification(target(), "addTags");
	for (auto node : addTagsTo)
	{
		if (auto astNode = DCast<Model::CompositeNode>(node))
		{
			auto tagExtension = astNode->extension<TagExtension>();
			treeManager->changeModificationTarget(astNode);
			tagExtension->tags()->append(new Model::Text{"foo"});
		}
	}
	treeManager->endModification();

	return result;
}

QList<Model::Text*> TagQuery::allTags(Model::Node* from)
{
	QList<Model::Text*> result;

	if (!from) from = target()->root();

	QList<Model::Node*> workStack{from};

	while (!workStack.empty())
	{
		auto node = workStack.takeLast();
		if (auto astNode = DCast<Model::CompositeNode>(node))
		{
			auto tagExtension = astNode->extension<TagExtension>();
			for (auto tag : *(tagExtension->tags()))
				result << tag;
		}
		workStack << node->children();
	}
	return result;
}

} /* namespace InformationScripting */
