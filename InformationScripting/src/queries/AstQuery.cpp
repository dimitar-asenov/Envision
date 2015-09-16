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
#include "QueryRegistry.h"

namespace InformationScripting {

const QStringList AstQuery::SCOPE_ARGUMENT_NAMES{"s", "scope"};
const QStringList AstQuery::NODETYPE_ARGUMENT_NAMES{"t", "type"};
const QStringList AstQuery::NAME_ARGUMENT_NAMES{"n", "name"};

AstQuery::AstQuery(QueryType type, Model::Node* target, QStringList args)
	: target_{target}, type_{type}
{
	args.prepend("AstQuery");
	argParser_.addOptions(
	{
		 {SCOPE_ARGUMENT_NAMES, "Scope argument", SCOPE_ARGUMENT_NAMES[1]},
		 {NODETYPE_ARGUMENT_NAMES, "AST Type argument", NODETYPE_ARGUMENT_NAMES[1]},
		 {NAME_ARGUMENT_NAMES, "Name of a symbol", NAME_ARGUMENT_NAMES[1]}
	});
	// Since all our options require values we don't want -abc to be interpreted as -a -b -c but as --abc
	argParser_.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	if (!argParser_.parse(args))
		qWarning() << "ASTQuery parse failure"; // TODO warn user

	QString scope = argParser_.value(SCOPE_ARGUMENT_NAMES[0]);
	if (scope == "g") scope_ = Scope::Global;
	else if (scope == "of") scope_ = Scope::Input;
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
		case QueryType::Generic:
			result = {genericQuery(input)};
			break;
		case QueryType::GenericToParent:
			if (argParser_.value(NODETYPE_ARGUMENT_NAMES[0]).size() > 0)
				result = {toParentType(input, argParser_.value(NODETYPE_ARGUMENT_NAMES[0]))};
			else
				result = input; // TODO warning for user ?
			break;
		default:
			Q_ASSERT(false);
	}

	return result;
}

void AstQuery::registerDefaultQueries()
{
	auto& registry = QueryRegistry::instance();
	registry.registerQueryConstructor("classes", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::Classes, target, args);
	});
	registry.registerQueryConstructor("methods", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::Methods, target, args);
	});
	registry.registerQueryConstructor("bases", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::BaseClasses, target, args);
	});
	registry.registerQueryConstructor("toClass", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::ToClass, target, args);
	});
	registry.registerQueryConstructor("callgraph", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::CallGraph, target, args);
	});
	registry.registerQueryConstructor("ast", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::Generic, target, args);
	});
	registry.registerQueryConstructor("toParent", [](Model::Node* target, QStringList args) {
		return new AstQuery(AstQuery::QueryType::GenericToParent, target, args);
	});
}

TupleSet AstQuery::classesQuery(QList<TupleSet> input)
{
	return typeQuery(input, "Class");
}

TupleSet AstQuery::methodsQuery(QList<TupleSet> input)
{
	return typeQuery(input, "Method");
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
	return toParentType(input, "Class");
}

TupleSet AstQuery::toParentType(QList<TupleSet> input, const QString& type)
{
	Q_ASSERT(input.size());
	auto ts = input.takeFirst();
	auto canBeParent = [type](const Tuple& t) {
		auto it = t.find("ast");
		if (it != t.end())
		{
			Model::Node* n = it->second;
			return n->firstAncestorOfType(type) != nullptr;
		}
		return false;
	};
	// TODO currently we remove all the converted nodes:
	auto tuples = ts.take(canBeParent);
	QSet<Model::Node*> parentNodes;
	for (auto tuple : tuples)
	{
		Model::Node* astNode = tuple["ast"];
		auto parentNode = astNode->firstAncestorOfType(type);
		parentNodes.insert(parentNode);
	}
	for (auto foundNode : parentNodes)
		ts.add({{"ast", foundNode}});
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

TupleSet AstQuery::genericQuery(QList<TupleSet> input)
{
	QString typeArgument = argParser_.value(NODETYPE_ARGUMENT_NAMES[0]);
	if (typeArgument.size() > 0) return typeQuery(input, typeArgument);
	return {};
}

TupleSet AstQuery::typeQuery(QList<TupleSet> input, QString type)
{
	TupleSet tuples;

	Q_ASSERT(!type.isEmpty());

	if (scope_ == Scope::Local)
		addNodesOfType(tuples, type, target_);
	else if (scope_ == Scope::Global)
		addNodesOfType(tuples, type);
	else if (scope_ == Scope::Input)
	{
		Q_ASSERT(input.size());
		tuples = input.takeFirst();

		// TODO add the possibility to keep the input nodes:
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple) addNodesOfType(tuples, type, t["ast"]);
	}
	return tuples;
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

void AstQuery::addNodesOfType(TupleSet& ts, const QString& typeName, Model::Node* from)
{
	if (!from) from = target_->manager()->root();
	auto allNodeOfType =  AllNodesOfType::allNodesOfType(from, typeName);
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

} /* namespace InformationScripting */
