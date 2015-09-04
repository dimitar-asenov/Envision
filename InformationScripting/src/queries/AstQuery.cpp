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

#include "AstQuery.h"

#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Project.h"

#include "../visitors/AllNodesOfType.h"

namespace InformationScripting {

AstQuery::AstQuery(QueryType type, Model::Node* target, QStringList args)
	: target_{target}, type_{type}
{
	if (args.size())
	{
		if (args[0] == "g") scope_ = Scope::Global;
		else if (args[0] == "of") scope_ = Scope::Input;
	}
}

QList<TupleSet> AstQuery::execute(QList<TupleSet> input)
{
	QList<TupleSet> result;
	switch (type_)
	{
		case QueryType::Classes:
			result = {classesQuery(input)};
			break;
		case QueryType::Methods:
			result = {methodsQuery(input)};
			break;
		case QueryType::BaseClasses:
			result = {baseClassesQuery(input)};
			break;
		case QueryType::ToClass:
			result = {toClassNode(input)};
			break;
		case QueryType::CallGraph:
			result = {callGraph(input)};
			break;
		default:
			Q_ASSERT(false);
	}

	return result;
}

TupleSet AstQuery::classesQuery(QList<TupleSet>)
{
	TupleSet tuples;
	if (scope_ == Scope::Local)
	{
		auto parentProject = target_->firstAncestorOfType<OOModel::Project>();
		for (auto childClass : *parentProject->classes())
			tuples.add({{"ast", childClass}});
	}
	else if (scope_ == Scope::Global)
	{
		addGlobalNodesOfType(tuples, "Class");
	}
	else if (scope_ == Scope::Input)
	{
		// TODO
	}
	return tuples;
}

TupleSet AstQuery::methodsQuery(QList<TupleSet> input)
{
	if (scope_ == Scope::Local)
	{
		TupleSet tuples;
		auto parentClass = target_->firstAncestorOfType<OOModel::Class>();
		for (auto method : *parentClass->methods())
			tuples.add({{"ast", method}});
		return tuples;
	}
	else if (scope_ == Scope::Global)
	{
		TupleSet tuples;
		addGlobalNodesOfType(tuples, "Method");
		return tuples;
	}
	else if (scope_ == Scope::Input)
	{
		Q_ASSERT(input.size());
		auto ts = input.takeFirst();

		auto canContainMethod = [](const Tuple& t) {
			if (t.size() == 1) {
				auto it = t.find("ast");
				if (it != t.end()) {
					Model::Node* astNode = it->second;
					return DCast<OOModel::Declaration>(astNode) != nullptr;
				}
			}
			return false;
		};

		// TODO for now we just remove the input node, this might not always be what we want
		auto tuple = ts.take(canContainMethod);
		for (const auto& t : tuple)
		{
			Model::Node* astNode = t["ast"];
			auto methods = AllNodesOfType::allNodesOfType(astNode, "Method");
			for (auto method : methods) ts.add({{"ast", method}});
		}
		return ts;
	}
	Q_ASSERT(false);
	return {};
}

TupleSet AstQuery::baseClassesQuery(QList<TupleSet>)
{
	// TODO handle input
	TupleSet ts;
	if (scope_ == Scope::Local)
	{
		OOModel::Class* parentClass = DCast<OOModel::Class>(target_);
		if (!parentClass) parentClass = target_->firstAncestorOfType<OOModel::Class>();

		NamedProperty namedClass{"childClass", parentClass};
		ts.add({namedClass});

		addBaseEdgesFor(parentClass, namedClass, ts);
	}
	else if (scope_ == Scope::Global)
	{
		// TODO
	}
	return ts;
}

TupleSet AstQuery::toClassNode(QList<TupleSet> input)
{
	Q_ASSERT(input.size());
	auto ts = input.takeFirst();
	auto canBeInClass = [](const Tuple& t) {
		auto it = t.find("ast");
		if (it != t.end())
		{
			Model::Node* n = it->second;
			return n->firstAncestorOfType<OOModel::Class>() != nullptr;
		}
		return false;
	};

	// TODO currently we remove all the converted nodes:
	auto tuples = ts.take(canBeInClass);
	QList<OOModel::Class*> classes;
	for (auto tuple : tuples)
	{
		Model::Node* astNode = tuple["ast"];
		auto classParent = astNode->firstAncestorOfType<OOModel::Class>();
		if (!classes.contains(classParent)) classes.push_back(classParent);
	}
	for (auto foundClass : classes)
		ts.add({{"ast", foundClass}});
	return ts;
}

TupleSet AstQuery::callGraph(QList<TupleSet>)
{
	TupleSet ts;
	if (scope_ == Scope::Local)
	{
		auto methodTarget = target_->firstAncestorOfType<OOModel::Method>();
		Q_ASSERT(methodTarget);
		QSet<OOModel::Method*> seenMethods{methodTarget};
		auto methods = methodTarget->callees().toList();
		addCallInformation(ts, methodTarget, methods);
		while (!methods.empty())
		{
			auto currentMethod = methods.takeLast();
			if (seenMethods.contains(currentMethod)) continue;
			seenMethods.insert(currentMethod);
			auto newCallees = currentMethod->callees().toList();
			addCallInformation(ts, currentMethod, newCallees);
			methods << newCallees;
		}
	}
	// TODO handle other cases, global propably doesn't make sense.
	return ts;
}

void AstQuery::addBaseEdgesFor(OOModel::Class* childClass, NamedProperty& classNode, TupleSet& ts)
{
	auto bases = childClass->directBaseClasses();
	for (auto base : bases)
	{
		NamedProperty baseNode{"baseClass", base};
		ts.add({baseNode});
		ts.add({{"base class", {}}, {classNode}, {baseNode}});
		addBaseEdgesFor(base, baseNode, ts);
	}
}

void AstQuery::addCallInformation(TupleSet& ts, OOModel::Method* method, QList<OOModel::Method*> callees)
{
	NamedProperty namedCaller{"caller", method};
	ts.add({{namedCaller}});
	for (auto callee : callees)
	{
		NamedProperty namedCallee{"callee", callee};
		ts.add({{namedCallee}});
		ts.add({{"calls", {}}, namedCaller, namedCallee});
	}
}

void AstQuery::addGlobalNodesOfType(TupleSet& ts, const QString& typeName)
{
	auto root = target_->manager()->root();
	auto allNodeOfType =  AllNodesOfType::allNodesOfType(root, typeName);
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

} /* namespace InformationScripting */
