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

#include "ModelBase/src/util/NameResolver.h"

#include "../visitors/AllNodesOfType.h"
#include "QueryRegistry.h"

namespace InformationScripting {

const QStringList AstQuery::SCOPE_ARGUMENT_NAMES{"s", "scope"};
const QStringList AstQuery::NODETYPE_ARGUMENT_NAMES{"t", "type"};
const QStringList AstQuery::NAME_ARGUMENT_NAMES{"n", "name"};
const QStringList AstQuery::ADD_AS_NAMES{"a", "addAs"};

AstQuery::AstQuery(ExecuteFunction<AstQuery> exec, Model::Node* target, QStringList args)
	: target_{target}, exec_{exec}
{
	args.prepend("AstQuery");
	argParser_.addOptions(
	{
		 {SCOPE_ARGUMENT_NAMES, "Scope argument", SCOPE_ARGUMENT_NAMES[1]},
		 {NODETYPE_ARGUMENT_NAMES, "AST Type argument", NODETYPE_ARGUMENT_NAMES[1]},
		 {NAME_ARGUMENT_NAMES, "Name of a symbol", NAME_ARGUMENT_NAMES[1]},
		 {ADD_AS_NAMES, "Add as relation or nodes", ADD_AS_NAMES[1], "relation"}
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
	return exec_(this, input);
}

void AstQuery::registerDefaultQueries()
{
	auto& registry = QueryRegistry::instance();
	registry.registerQueryConstructor("classes", [](Model::Node* target, QStringList args) {
		setTypeTo(args, "Class");
		return new AstQuery(&AstQuery::genericQuery, target, args);
	});
	registry.registerQueryConstructor("methods", [](Model::Node* target, QStringList args) {
		setTypeTo(args, "Method");
		return new AstQuery(&AstQuery::genericQuery, target, args);
	});
	registry.registerQueryConstructor("bases", [](Model::Node* target, QStringList args) {
		return new AstQuery(&AstQuery::baseClassesQuery, target, args);
	});
	registry.registerQueryConstructor("toClass", [](Model::Node* target, QStringList args) {
		setTypeTo(args, "Class");
		return new AstQuery(&AstQuery::toParentType, target, args);
	});
	registry.registerQueryConstructor("callgraph", [](Model::Node* target, QStringList args) {
		return new AstQuery(&AstQuery::callGraph, target, args);
	});
	registry.registerQueryConstructor("ast", [](Model::Node* target, QStringList args) {
		return new AstQuery(&AstQuery::genericQuery, target, args);
	});
	registry.registerQueryConstructor("toParent", [](Model::Node* target, QStringList args) {
		return new AstQuery(&AstQuery::toParentType, target, args);
	});
	registry.registerQueryConstructor("uses", [](Model::Node* target, QStringList args) {
		return new AstQuery(&AstQuery::usesQuery, target, args);
	});
}

void AstQuery::setTypeTo(QStringList& args, QString type)
{
	bool set = false;
	for (auto& arg : args)
	{
		if (arg.startsWith("-" + NODETYPE_ARGUMENT_NAMES[0]))
		{
			arg.replace(QRegularExpression("=.*"), "=" + type);
			set = true;
		}
	}
	if (!set)
	{
		args.append(QString("-%1=%2").arg(NODETYPE_ARGUMENT_NAMES[0], type));
	}
}

QList<TupleSet> AstQuery::baseClassesQuery(QList<TupleSet>)
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

		adaptOutputForRelation(ts, "base class", {"baseClass"});
	}
	else if (scope_ == Scope::Global)
	{
		// TODO
	}
	return {ts};
}

QList<TupleSet> AstQuery::toParentType(QList<TupleSet> input)
{
	Q_ASSERT(input.size());
	QString type = argParser_.value(NODETYPE_ARGUMENT_NAMES[0]);
	Q_ASSERT(type.size() > 0); // TODO should be a warning for the user.

	auto ts = input.takeFirst();
	Model::SymbolMatcher matcher = matcherFor(type);

	auto haveMatchingParent = [matcher](const Tuple& t) {
		auto it = t.find("ast");
		if (it != t.end())
		{
			Model::Node* n = it->second;
			return n->firstAncestorOfType(matcher) != nullptr;
		}
		return false;
	};
	auto tuples = ts.tuples(haveMatchingParent);
	for (auto tuple : tuples)
	{
		Model::Node* astNode = tuple["ast"];
		auto parentNode = astNode->firstAncestorOfType(matcher);
		ts.add({{"parentOfType", {}}, {"childNode", astNode}, {"parentNode", parentNode}});
	}
	adaptOutputForRelation(ts, "parentOfType", {"parentNode"});
	return {ts};
}

QList<TupleSet> AstQuery::callGraph(QList<TupleSet>)
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

		adaptOutputForRelation(ts, "calls", {"caller", "callee"});
	}
	// TODO handle other cases, global propably doesn't make sense.
	return {ts};
}

QList<TupleSet> AstQuery::genericQuery(QList<TupleSet> input)
{
	QString typeArgument = argParser_.value(NODETYPE_ARGUMENT_NAMES[0]);
	QString nameArgument = argParser_.value(NAME_ARGUMENT_NAMES[0]);
	if (nameArgument.size() > 0) return nameQuery(input, nameArgument);
	else if (typeArgument.size() > 0) return typeQuery(input, typeArgument);
	return {};
}

QList<TupleSet> AstQuery::typeQuery(QList<TupleSet> input, QString type)
{
	TupleSet tuples;

	Q_ASSERT(!type.isEmpty());
	Model::SymbolMatcher matcher = matcherFor(type);

	if (scope_ == Scope::Local)
		addNodesOfType(tuples, matcher, target_);
	else if (scope_ == Scope::Global)
		addNodesOfType(tuples, matcher);
	else if (scope_ == Scope::Input)
	{
		Q_ASSERT(input.size());
		tuples = input.takeFirst();

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple) addNodesOfType(tuples, matcher, t["ast"]);
	}
	return {tuples};
}

QList<TupleSet> AstQuery::nameQuery(QList<TupleSet> input, QString name)
{
	TupleSet tuples;

	QList<QPair<QString, Model::Node*>> matchingNodes;

	if (scope_ == Scope::Local)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1, target_);
	else if (scope_ == Scope::Global)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1);
	else if (scope_ == Scope::Input)
	{
		Q_ASSERT(input.size());
		tuples = input.takeFirst();

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple) matchingNodes << Model::NameResolver::mostLikelyMatches(name, -1, t["ast"]);
	}
	// If we have a type argument filter the results:
	const QString type = argParser_.value(NODETYPE_ARGUMENT_NAMES[0]);
	Model::SymbolMatcher matcher = matcherFor(type);
	for (auto matchingNode : matchingNodes)
		if (type.isEmpty() || matcher.matches(matchingNode.second->typeName()))
			tuples.add({{"ast", matchingNode.second}});

	return {tuples};
}

QList<TupleSet> AstQuery::usesQuery(QList<TupleSet> input)
{
	TupleSet result;
	QHash<Model::Node*, QList<Model::Reference*>> references;

	if (scope_ == Scope::Local)
		references[target_] = AllNodesOfType::allNodesOfType<Model::Reference>(target_);
	else if (scope_ == Scope::Global)
		references[target_->root()] = AllNodesOfType::allNodesOfType<Model::Reference>(target_->root());
	else if (scope_ == Scope::Input)
	{
		Q_ASSERT(input.size());
		auto tuples = input.takeFirst();

		auto tuple = tuples.tuples("ast");
		for (const auto& t : tuple) references[t["ast"]] = AllNodesOfType::allNodesOfType<Model::Reference>(t["ast"]);

		// Keep all non ast nodes
		result = tuples;
	}

	auto typeMatcher = matcherFor(argParser_.value(NODETYPE_ARGUMENT_NAMES[0]));
	auto nameMatcher = matcherFor(argParser_.value(NAME_ARGUMENT_NAMES[0]));

	QHash<Model::Node*, QList<Model::Node*>> referenceTargets;
	for (auto it = references.begin(); it != references.end(); ++it)
	{
		for (auto reference : it.value())
		{
			if (auto target = reference->target())
			{
				if ((typeMatcher.matchPattern().isEmpty() || typeMatcher.matches(target->typeName())) &&
					  (nameMatcher.matchPattern().isEmpty() || nameMatcher.matches(target->symbolName())))
					referenceTargets[it.key()] << target;
			}
		}
	}

	for (auto it = referenceTargets.begin(); it != referenceTargets.end(); ++it)
		for (auto node : it.value())
			result.add({{"uses", {}}, {"user", it.key()}, {"used", node}});

	adaptOutputForRelation(result, "uses", {"user"});

	return {result};
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

void AstQuery::addNodesOfType(TupleSet& ts, const Model::SymbolMatcher& matcher, Model::Node* from)
{
	if (!from) from = target_->root();
	auto allNodeOfType =  AllNodesOfType::allNodesOfType(from, matcher);
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

Model::SymbolMatcher AstQuery::matcherFor(const QString& text)
{
	if (text.contains("*")) return {new QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard)};
	return {text};
}

void AstQuery::adaptOutputForRelation(TupleSet& tupleSet, const QString& relationName,
												  const QStringList& keepProperties)
{
	if (argParser_.value(ADD_AS_NAMES[0]) != "relation")
	{
		// remove all ast properties first
		tupleSet.take("ast");
		// add the tuples from the relation
		auto tuples = tupleSet.take(relationName);
		for (auto tuple : tuples)
			for (const auto& keepProperty : keepProperties)
				tupleSet.add({{"ast", tuple[keepProperty]}});
	}
}

} /* namespace InformationScripting */
