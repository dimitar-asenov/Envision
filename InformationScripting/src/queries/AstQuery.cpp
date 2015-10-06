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
#include "OOModel/src/expressions/types/TypeExpression.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/PointerTypeExpression.h"
#include "OOModel/src/expressions/types/ReferenceTypeExpression.h"
#include "OOModel/src/expressions/types/AutoTypeExpression.h"
#include "OOModel/src/expressions/types/TypeQualifierExpression.h"
#include "OOModel/src/expressions/types/FunctionTypeExpression.h"

#include "ModelBase/src/util/NameResolver.h"
#include "ModelBase/src/util/SymbolMatcher.h"

#include "../visitors/AllNodesOfType.h"
#include "QueryRegistry.h"

namespace InformationScripting {

const QStringList AstQuery::NODETYPE_ARGUMENT_NAMES{"t", "type"};
const QStringList AstQuery::NAME_ARGUMENT_NAMES{"n", "name"};
const QStringList AstQuery::ADD_AS_NAMES{"a", "addAs"};

AstQuery::AstQuery(ExecuteFunction exec, Model::Node* target, QStringList args)
	: ScopedArgumentQuery{target, {
		 {NODETYPE_ARGUMENT_NAMES, "AST Type argument", NODETYPE_ARGUMENT_NAMES[1]},
		 {NAME_ARGUMENT_NAMES, "Name of a symbol", NAME_ARGUMENT_NAMES[1]},
		 {ADD_AS_NAMES, "Add as relation or nodes", ADD_AS_NAMES[1], "relation"}
		}, QStringList("AstQuery") + args}, exec_{exec}
{}

TupleSet AstQuery::execute(TupleSet input)
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
	registry.registerQueryConstructor("type", [](Model::Node* target, QStringList args) {
		return new AstQuery(&AstQuery::typeFilter, target, args);
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

TupleSet AstQuery::baseClassesQuery(TupleSet)
{
	// TODO handle input
	TupleSet ts;
	if (scope() == Scope::Local)
	{
		OOModel::Class* parentClass = DCast<OOModel::Class>(target());
		if (!parentClass) parentClass = target()->firstAncestorOfType<OOModel::Class>();

		NamedProperty namedClass{"childClass", parentClass};
		ts.add({namedClass});

		addBaseEdgesFor(parentClass, namedClass, ts);

		adaptOutputForRelation(ts, "base class", {"baseClass"});
	}
	else if (scope() == Scope::Global)
	{
		// TODO
	}
	return ts;
}

TupleSet AstQuery::toParentType(TupleSet input)
{
	QString type = argument(NODETYPE_ARGUMENT_NAMES[0]);
	Q_ASSERT(type.size() > 0); // TODO should be a warning for the user.

	auto ts = input;
	Model::SymbolMatcher matcher = Model::SymbolMatcher::guessMatcher(type);

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
	return ts;
}

TupleSet AstQuery::callGraph(TupleSet)
{
	TupleSet ts;
	if (scope() == Scope::Local)
	{
		auto methodTarget = target()->firstAncestorOfType<OOModel::Method>();
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
	return ts;
}

TupleSet AstQuery::genericQuery(TupleSet input)
{
	QString typeArgument = argument(NODETYPE_ARGUMENT_NAMES[0]);
	QString nameArgument = argument(NAME_ARGUMENT_NAMES[0]);
	if (nameArgument.size() > 0) return nameQuery(input, nameArgument);
	else if (typeArgument.size() > 0) return typeQuery(input, typeArgument);
	return {};
}

TupleSet AstQuery::typeQuery(TupleSet input, QString type)
{
	TupleSet tuples;

	Q_ASSERT(!type.isEmpty());
	Model::SymbolMatcher matcher = Model::SymbolMatcher::guessMatcher(type);

	if (scope() == Scope::Local)
		addNodesOfType(tuples, matcher, target());
	else if (scope() == Scope::Global)
		addNodesOfType(tuples, matcher);
	else if (scope() == Scope::Input)
	{
		tuples = input;

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple) addNodesOfType(tuples, matcher, t["ast"]);
	}
	return tuples;
}

TupleSet AstQuery::nameQuery(TupleSet input, QString name)
{
	TupleSet tuples;

	QList<QPair<QString, Model::Node*>> matchingNodes;

	if (scope() == Scope::Local)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1, target());
	else if (scope() == Scope::Global)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1);
	else if (scope() == Scope::Input)
	{
		tuples = input;

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple) matchingNodes << Model::NameResolver::mostLikelyMatches(name, -1, t["ast"]);
	}
	// If we have a type argument filter the results:
	const QString type = argument(NODETYPE_ARGUMENT_NAMES[0]);
	Model::SymbolMatcher matcher = Model::SymbolMatcher::guessMatcher(type);
	for (auto matchingNode : matchingNodes)
		if (type.isEmpty() || matcher.matches(matchingNode.second->typeName()))
			tuples.add({{"ast", matchingNode.second}});

	return tuples;
}

TupleSet AstQuery::usesQuery(TupleSet input)
{
	TupleSet result;
	QHash<Model::Node*, QList<Model::Reference*>> references;

	if (scope() == Scope::Local)
		references[target()] = AllNodesOfType::allNodesOfType<Model::Reference>(target());
	else if (scope() == Scope::Global)
		references[target()->root()] = AllNodesOfType::allNodesOfType<Model::Reference>(target()->root());
	else if (scope() == Scope::Input)
	{
		auto tuples = input;

		auto tuple = tuples.tuples("ast");
		for (const auto& t : tuple) references[t["ast"]] = AllNodesOfType::allNodesOfType<Model::Reference>(t["ast"]);

		// Keep all non ast nodes
		result = tuples;
	}

	auto typeMatcher = Model::SymbolMatcher::guessMatcher(argument(NODETYPE_ARGUMENT_NAMES[0]));
	auto nameMatcher = Model::SymbolMatcher::guessMatcher(argument(NAME_ARGUMENT_NAMES[0]));

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

	return result;
}

TupleSet AstQuery::typeFilter(TupleSet input)
{
	using SymbolType = Model::Node::SymbolType;

	SymbolType expectedSymbolType = SymbolType::VARIABLE;
	QString expectedType;
	QStringList arguments;
	// NOTE: To use spaces in this argument use quotes!
	// QCommandLineParser removes the entered spaces automatically
	// FIXME don't use nodetype argument as it is the actual type:
	auto typeArgument = argument(NODETYPE_ARGUMENT_NAMES[1]);
	// Remove quotes if there are
	if (typeArgument.startsWith("\"")) typeArgument = typeArgument.mid(1);
	if (typeArgument.endsWith("\"")) typeArgument = typeArgument.left(typeArgument.size()-1);

	auto parts = typeArgument.split("(", QString::SkipEmptyParts);
	expectedType = parts[0];
	if (parts.size() > 1)
	{
		expectedSymbolType = SymbolType::METHOD;
		auto writtenArgs = parts[1].left(parts[1].size() - 1); // Drop the closing parent
		arguments = writtenArgs.split(",", QString::SkipEmptyParts);
	}

	// Lambda which returns true for all tuples which don't match the expected type.
	auto toRemove = [expectedSymbolType, expectedType, arguments](const Tuple& t) {
		if (t.tag() == "ast")
		{
			Model::Node* astNode = t["ast"];
			auto symbolTypes = astNode->symbolType();

			if (symbolTypes.testFlag(expectedSymbolType))
				return !matchesExpectedType(astNode, expectedSymbolType, expectedType, arguments);
		}
		return true;
	};

	input.take(toRemove);
	return input;
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
	if (!from) from = target()->root();
	auto allNodeOfType =  AllNodesOfType::allNodesOfType(from, matcher);
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

void AstQuery::adaptOutputForRelation(TupleSet& tupleSet, const QString& relationName,
												  const QStringList& keepProperties)
{
	if (argument(ADD_AS_NAMES[0]) != "relation")
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

bool AstQuery::matchesExpectedType(Model::Node* node, Model::Node::SymbolType symbolType,
											  const QString& expectedType, const QStringList& args)
{
	using SymbolType = Model::Node::SymbolType;
	if (SymbolType::VARIABLE == symbolType)
	{
		if (auto varDecl = DCast<OOModel::VariableDeclaration>(node))
			return stringifyType(varDecl->typeExpression()) == expectedType;
	}
	else
	{
		if (auto methodDecl = DCast<OOModel::Method>(node))
		{
			if (args.size() != methodDecl->arguments()->size()) return false;
			// TODO: here we support only one return value:
			if ((methodDecl->results()->size() == 0 && (expectedType.isEmpty() || expectedType == "void"))
				 || (methodDecl->results()->size() > 0 &&
					  stringifyType(methodDecl->results()->at(0)->typeExpression()) == expectedType)
				 || expectedType == "_")
			{
				// return type matches check arguments:
				bool matches = true;
				for (int i = 0; i < args.size() && matches; ++i)
				{
					if (args[i] == "_") continue;
					matches = stringifyType(methodDecl->arguments()->at(i)->typeExpression()) == args[i];
				}
				return matches;
			}
		}
	}
	return false;
}

QString AstQuery::stringifyType(OOModel::Expression* expression)
{
	using namespace OOModel;
	if (!expression) return {};
	if (auto e = DCast<ArrayTypeExpression>(expression))
		return stringifyType(e->typeExpression()) + "[" + stringifyType(e->fixedSize()) + "]";
	else if (auto e = DCast<ReferenceTypeExpression>(expression)) return stringifyType(e) + "&";
	else if (auto e = DCast<PointerTypeExpression>(expression)) return stringifyType(e);
	else if (auto e = DCast<ClassTypeExpression>(expression)) return stringifyType(e->typeExpression());
	else if (auto e = DCast<PrimitiveTypeExpression>(expression))
	{
		switch (e->typeValue())
		{
			case PrimitiveTypeExpression::PrimitiveTypes::INT: return "int";
			case PrimitiveTypeExpression::PrimitiveTypes::LONG: return "long";
			case PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT: return "unsigned int";
			case PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG: return "unsigned long";
			case PrimitiveTypeExpression::PrimitiveTypes::FLOAT: return "float";
			case PrimitiveTypeExpression::PrimitiveTypes::DOUBLE: return "double";
			case PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN: return "boolean";
			case PrimitiveTypeExpression::PrimitiveTypes::CHAR: return "char";
			case PrimitiveTypeExpression::PrimitiveTypes::VOID: return "void";
			default: Q_ASSERT(false);
		}
	}
	else if (auto e = DCast<TypeQualifierExpression>(expression))
	{
		QString qualifier;
		switch (e->qualifier())
		{
			case TypeQualifierExpression::Qualifier::CONST: qualifier = "const"; break;
			case TypeQualifierExpression::Qualifier::VOLATILE: qualifier = "volatile"; break;
			default: Q_ASSERT(false);
		}
		return qualifier + " " + stringifyType(e->typeExpression());
	}
	else if (DCast<AutoTypeExpression>(expression)) return "auto";
	else if (DCast<FunctionTypeExpression>(expression)) Q_ASSERT(false);
	else if (auto e = DCast<ReferenceExpression>(expression))
	{
		QString result;
		if (e->prefix()) result += stringifyType(e->prefix()) + ".";
		result += e->name();
		// TODO: handle type arguments
		return result;
	}
	return {};
}

} /* namespace InformationScripting */
