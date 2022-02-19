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

#include "OOInteraction/src/string_offset_providers/StringComponents.h"

#include "ModelBase/src/util/NameResolver.h"
#include "ModelBase/src/util/SymbolMatcher.h"

#include "../query_framework/QueryRegistry.h"

namespace InformationScripting {

const QStringList AstQuery::NODETYPE_ARGUMENT_NAMES{"t", "type"};
const QStringList AstQuery::NAME_ARGUMENT_NAMES{"n", "name"};
const QStringList AstQuery::NODES_ARGUMENT_NAMES{"nodes"};
const QStringList AstQuery::RELATION_ARGUMENT_NAMES{"r", "relation"};
const QStringList AstQuery::ATTRIBUTE_NAME_NAMES{"at", "attribute"};
const QStringList AstQuery::TOP_LEVEL_ARGUMENT_NAMES{"tl", "topLevel"};

AstQuery::AstQuery(Model::Node* target, QStringList args, ExecuteFunction exec, std::vector<ArgumentRule> argumentRules)
	: LinearQuery{target}, arguments_{{
		 {NODETYPE_ARGUMENT_NAMES, "AST Type argument", NODETYPE_ARGUMENT_NAMES[1]},
		 {NAME_ARGUMENT_NAMES, "Name of a symbol", NAME_ARGUMENT_NAMES[1]},
		 {ATTRIBUTE_NAME_NAMES, "Attribute to search from", ATTRIBUTE_NAME_NAMES[1]},
		 QCommandLineOption{NODES_ARGUMENT_NAMES},
		 QCommandLineOption{RELATION_ARGUMENT_NAMES},
		 QCommandLineOption{TOP_LEVEL_ARGUMENT_NAMES}
		}, args, true}, exec_{exec}
{
	for (const auto& rule : argumentRules)
		rule.check(arguments_);
}

Optional<TupleSet> AstQuery::executeLinear(TupleSet input)
{
	return exec_(this, input);
}

void AstQuery::registerDefaultQueries()
{
	QueryRegistry::registerQuery<AstQuery>("bases", &AstQuery::baseClassesQuery);
	QueryRegistry::registerQuery<AstQuery>("callgraph", &AstQuery::callGraph);
	QueryRegistry::registerQuery<AstQuery>("ast", &AstQuery::genericQuery,
		std::vector<ArgumentRule>{{ArgumentRule::RequireOneOf,
											{{NODETYPE_ARGUMENT_NAMES[1]}, {NAME_ARGUMENT_NAMES[1]}}}});
	QueryRegistry::registerQuery<AstQuery>("toParent", &AstQuery::toParentType,
		std::vector<ArgumentRule>{{ArgumentRule::RequireAll, {{NODETYPE_ARGUMENT_NAMES[1]}}}});
	QueryRegistry::registerQuery<AstQuery>("definitions", &AstQuery::defintionsQuery,
		std::vector<ArgumentRule>{{ArgumentRule::RequireOneOf,
											{{NODETYPE_ARGUMENT_NAMES[1]}, {NAME_ARGUMENT_NAMES[1]}}}});
	QueryRegistry::registerQuery<AstQuery>("type", &AstQuery::typeFilter,
		std::vector<ArgumentRule>{{ArgumentRule::RequireAll, {{NODETYPE_ARGUMENT_NAMES[1]}}}});
	QueryRegistry::registerQuery<AstQuery>("attribute", &AstQuery::attribute,
		std::vector<ArgumentRule>{{ArgumentRule::RequireAll, {{ATTRIBUTE_NAME_NAMES[1]}}}});
	QueryRegistry::registerQuery<AstQuery>("usages", &AstQuery::usagesQuery);
}

Optional<TupleSet> AstQuery::baseClassesQuery(TupleSet input)
{
	QList<OOModel::Class*> childClasses;
	TupleSet ts;
	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
	{
		OOModel::Class* parentClass = DCast<OOModel::Class>(target());
		if (!parentClass) parentClass = target()->firstAncestorOfType<OOModel::Class>();
		childClasses.push_back(parentClass);
	}
	else if (scope == ArgumentParser::Scope::Global)
	{
		childClasses = Model::Node::childrenOfType<OOModel::Class>(target()->root());
	}
	else if (scope == ArgumentParser::Scope::Input)
	{
		auto tuple = input.tuples("ast");
		for (const auto& t : tuple) childClasses << Model::Node::childrenOfType<OOModel::Class>(t["ast"]);
	}

	for (auto childClass : childClasses)
	{
		NamedProperty namedClass{"childClass", childClass};
		addBaseEdgesFor(childClass, namedClass, ts);

		if (arguments_.isArgumentSet(NODES_ARGUMENT_NAMES[0]))
			outputAsAST(ts, "baseclass", {"baseClass"});
	}
	return ts;
}

Optional<TupleSet> AstQuery::toParentType(TupleSet input)
{
	QString type = arguments_.argument(NODETYPE_ARGUMENT_NAMES[0]);
	Q_ASSERT(type.size() > 0);

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
	if (tuples.isEmpty()) return {ts, "No input nodes found which have a parent of type: " + type};

	for (auto tuple : tuples)
	{
		Model::Node* astNode = tuple["ast"];
		auto parentNode = astNode->firstAncestorOfType(matcher);
		ts.add({"parentOfType", {{"childNode", astNode}, {"parentNode", parentNode}}});
	}
	// Per default we want to parent to output nodes.
	if (!arguments_.isArgumentSet(RELATION_ARGUMENT_NAMES[1]))
		outputAsAST(ts, "parentOfType", {"parentNode"});
	return ts;
}

Optional<TupleSet> AstQuery::callGraph(TupleSet input)
{
	TupleSet result;

	auto addCallgraphFor = [&result](const QList<OOModel::Method*>& methods)
	{
		for (auto method : methods)
		{
			Q_ASSERT(method);
			QSet<OOModel::Method*> seenMethods{method};
			auto calleesSet = method->callees();
			QList<OOModel::Method*> callees{calleesSet.begin(), calleesSet.end()};
			addCallInformation(result, method, callees);
			while (!callees.empty())
			{
				auto currentMethod = callees.takeLast();
				if (seenMethods.contains(currentMethod)) continue;
				seenMethods.insert(currentMethod);
				auto newCalleesSet = currentMethod->callees();
				QList<OOModel::Method*> newCallees{newCalleesSet.begin(), newCalleesSet.end()};
				addCallInformation(result, currentMethod, newCallees);
				callees << newCallees;
			}
		}
	};

	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
	{
		if (auto method = DCast<OOModel::Method>(target())) addCallgraphFor({method});
		else return {"Callgraph does only work on method nodes"};
	}
	else if (scope == ArgumentParser::Scope::Global)
	{
		addCallgraphFor(Model::Node::childrenOfType<OOModel::Method>(target()->root()));
	}
	else if (scope == ArgumentParser::Scope::Input)
	{
		// Keep input nodes
		result = input;
		QList<OOModel::Method*> methodsInInput;
		for (const auto& astTuple : input.tuples("ast"))
		{
			Model::Node* astNode = astTuple["ast"];
			if (auto method = DCast<OOModel::Method>(astNode)) methodsInInput.push_back(method);
		}
		if (methodsInInput.size() == 0) return {result, "Called callgraph without methods in input"};
		else addCallgraphFor(methodsInInput);
	}
	if (arguments_.isArgumentSet(NODES_ARGUMENT_NAMES[0]))
		outputAsAST(result, "calls", {"caller", "callee"});
	return result;
}

Optional<TupleSet> AstQuery::genericQuery(TupleSet input)
{
	QString typeArgument = arguments_.argument(NODETYPE_ARGUMENT_NAMES[0]);
	QString nameArgument = arguments_.argument(NAME_ARGUMENT_NAMES[0]);
	if (nameArgument.size() > 0) return nameQuery(input, nameArgument);
	else if (typeArgument.size() > 0) return typeQuery(input, typeArgument);
	Q_ASSERT(false);
	return {"Generic Error"};
}

Optional<TupleSet> AstQuery::typeQuery(TupleSet input, QString type)
{
	TupleSet tuples;

	Q_ASSERT(!type.isEmpty());
	Model::SymbolMatcher matcher = Model::SymbolMatcher::guessMatcher(type);

	bool onlyAddTopLevelNodes = arguments_.isArgumentSet(TOP_LEVEL_ARGUMENT_NAMES[0]);

	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
		addNodesOfType(tuples, matcher, target(), onlyAddTopLevelNodes);
	else if (scope == ArgumentParser::Scope::Global)
		addNodesOfType(tuples, matcher, nullptr, onlyAddTopLevelNodes);
	else if (scope == ArgumentParser::Scope::Input)
	{
		tuples = input;

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple) addNodesOfType(tuples, matcher, t["ast"], onlyAddTopLevelNodes);
	}
	return tuples;
}

Optional<TupleSet> AstQuery::nameQuery(TupleSet input, QString name)
{
	Q_ASSERT(!name.isEmpty());

	TupleSet tuples;

	QList<QPair<QString, Model::Node*>> matchingNodes;

	auto suggestable = [](Model::Node::SymbolTypes symbolType) {
		using SymbolType = Model::Node::SymbolType;
		return symbolType == SymbolType::METHOD || symbolType == SymbolType::CONTAINER || symbolType == SymbolType::VARIABLE;
	};

	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1, target(), suggestable);
	else if (scope == ArgumentParser::Scope::Global)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1, nullptr, suggestable);
	else if (scope == ArgumentParser::Scope::Input)
	{
		tuples = input;

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple)
			matchingNodes << Model::NameResolver::mostLikelyMatches(name, -1, t["ast"], suggestable);
	}
	// If we have a type argument filter the results:
	const QString type = arguments_.argument(NODETYPE_ARGUMENT_NAMES[0]);
	Model::SymbolMatcher matcher = Model::SymbolMatcher::guessMatcher(type);
	for (auto matchingNode : matchingNodes)
		if (type.isEmpty() || matcher.matches(matchingNode.second->typeName()))
			tuples.add({{"ast", matchingNode.second}});

	return tuples;
}

Optional<TupleSet> AstQuery::defintionsQuery(TupleSet input)
{
	TupleSet result;
	QHash<Model::Node*, QList<Model::Reference*>> references;

	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
		references[target()] = Model::Node::childrenOfType<Model::Reference>(target());
	else if (scope == ArgumentParser::Scope::Global)
		references[target()->root()] = Model::Node::childrenOfType<Model::Reference>(target()->root());
	else if (scope == ArgumentParser::Scope::Input)
	{
		auto tuples = input;

		auto tuple = tuples.tuples("ast");
		for (const auto& t : tuple) references[t["ast"]] = Model::Node::childrenOfType<Model::Reference>(t["ast"]);

		// Keep all non ast nodes
		result = tuples;
	}

	auto typeMatcher = Model::SymbolMatcher::guessMatcher(arguments_.argument(NODETYPE_ARGUMENT_NAMES[0]));
	auto nameMatcher = Model::SymbolMatcher::guessMatcher(arguments_.argument(NAME_ARGUMENT_NAMES[0]));

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
			result.add({"definition", {{"reference", it.key()}, {"definition", node}}});

	if (arguments_.isArgumentSet(NODES_ARGUMENT_NAMES[0]))
		outputAsAST(result, "definition", {"definition"});

	return result;
}

Optional<TupleSet> AstQuery::typeFilter(TupleSet input)
{
	using SymbolType = Model::Node::SymbolType;

	SymbolType expectedSymbolType = SymbolType::VARIABLE;
	QString expectedType;
	QStringList arguments;
	TupleSet result;

	// NOTE: To use spaces in this argument use quotes!
	// QCommandLineParser removes the entered spaces automatically
	// NOTE: here the type argument has a different meaning than in the other queries:
	auto typeArgument = arguments_.argument(NODETYPE_ARGUMENT_NAMES[1]);
	// Remove quotes if there are
	if (typeArgument.startsWith("\"")) typeArgument = typeArgument.mid(1);
	if (typeArgument.endsWith("\"")) typeArgument = typeArgument.left(typeArgument.size()-1);

	auto parts = typeArgument.split("(", Qt::SkipEmptyParts);
	expectedType = parts[0];
	if (parts.size() > 1)
	{
		expectedSymbolType = SymbolType::METHOD;
		auto writtenArgs = parts[1].left(parts[1].size() - 1); // Drop the closing parent
		arguments = writtenArgs.split(",", Qt::SkipEmptyParts);
	}

	// Lambda which returns true for all nodes which match the expected type.
	auto keepNode = [expectedSymbolType, expectedType, arguments](Model::Node* node) {
		auto symbolTypes = node->symbolType();

		if (symbolTypes.testFlag(expectedSymbolType))
			return matchesExpectedType(node, expectedSymbolType, expectedType, arguments);
		return false;
	};

	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
		addNodesForWhich(result, keepNode, target());
	else if (scope == ArgumentParser::Scope::Global)
		addNodesForWhich(result, keepNode);
	else if (scope == ArgumentParser::Scope::Input)
	{
		result = input;

		// Note here we remove the input nodes.
		auto tuples = result.take("ast");
		for (const auto& t : tuples) addNodesForWhich(result, keepNode, t["ast"]);
	}

	return result;
}

Optional<TupleSet> AstQuery::attribute(TupleSet input)
{
	const QString attributeName = arguments_.argument(ATTRIBUTE_NAME_NAMES[1]);
	Q_ASSERT(!attributeName.isEmpty());

	std::vector<Model::Node*> foundAttributeNodes;
	auto findAttribute = [&attributeName, &foundAttributeNodes](Model::Node* node) {
		if (auto compositeNode = DCast<Model::CompositeNode>(node))
			if (compositeNode->hasAttribute(attributeName))
				foundAttributeNodes.push_back(compositeNode->get(attributeName));
	};

	TupleSet result;
	auto scope = arguments_.scope(this);
	if (scope == ArgumentParser::Scope::Local)
	{
		findAttribute(target());
	}
	else if (scope == ArgumentParser::Scope::Global)
	{
		auto nodesWithAttribute = Model::Node::childrenWhich(target()->root(), [&attributeName](Model::Node* node) {
			if (auto composite = DCast<Model::CompositeNode>(node))
				return composite->hasAttribute(attributeName);
			return false;
		});
		for (auto node : nodesWithAttribute)
			findAttribute(node);
	}
	else if (scope == ArgumentParser::Scope::Input)
	{
		auto tuples = input;

		// remove all existing ast nodes
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple)
			findAttribute(t["ast"]);

		// Keep all non ast nodes
		result = tuples;
	}
	for (auto node : foundAttributeNodes)
		result.add({{"ast", node}});
	return result;
}

Optional<TupleSet> AstQuery::usagesQuery(TupleSet input)
{
	TupleSet result = input;

	auto symbolDefiningNodeTuples = input.tuples([](const Tuple& t) {
		return t.tag() == "ast" && static_cast<Model::Node*>(t["ast"])->definesSymbol();
	});

	// TODO: This is quite an expensive operation, especially if this query is ran often.
	// We probably need a more general mechanism to compute/cache reverse dependencies in Envision.
	auto allReferences = Model::Node::childrenOfType<Model::Reference>(target()->root());

	for (const auto& tuple : symbolDefiningNodeTuples)
	{
		Model::Node* definitionNode = tuple["ast"];
		for (auto ref : allReferences)
			if (ref->target() == definitionNode)
				result.add({"usage", {{"definition", definitionNode}, {"use", ref}}});
	}

	if (arguments_.isArgumentSet(NODES_ARGUMENT_NAMES[0]))
		outputAsAST(result, "usage", {"use"});

	return result;
}

void AstQuery::addBaseEdgesFor(OOModel::Class* childClass, NamedProperty& classNode, TupleSet& ts)
{
	auto bases = childClass->directBaseClasses();
	for (auto base : bases)
	{
		NamedProperty baseNode{"baseClass", base};
		ts.add({"baseclass", {{classNode}, {"baseClass", base}}});
		addBaseEdgesFor(base, baseNode, ts);
	}
}

void AstQuery::addCallInformation(TupleSet& ts, OOModel::Method* method, QList<OOModel::Method*> callees)
{
	for (auto callee : callees)
		ts.add({"calls", {{"caller", method}, {"callee", callee}}});
}

void AstQuery::addNodesOfType(TupleSet& ts, const Model::SymbolMatcher& matcher,
										Model::Node* from, bool topLevelNodeOnly)
{
	if (!from) from = target()->root();
	auto allNodeOfType =  Model::Node::childrenWhich(from,
		[matcher](Model::Node* node) {
				if (matcher.isFixedString())
					return node->isSubtypeOf(matcher.matchPattern());
				return matcher.matches(node->typeName());
		}, topLevelNodeOnly
	);
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

template <typename Predicate>
void AstQuery::addNodesForWhich(TupleSet& ts, Predicate holds, Model::Node* from)
{
	if (!from) from = target()->root();
	auto allNodeOfType =  Model::Node::childrenWhich(from, holds);
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

void AstQuery::outputAsAST(TupleSet& tupleSet, const QString& relationName,
												  const QStringList& keepProperties)
{
	// remove all ast properties first
	tupleSet.take("ast");
	// add the tuples from the relation
	auto tuples = tupleSet.take(relationName);
	for (auto tuple : tuples)
		for (const auto& keepProperty : keepProperties)
			tupleSet.add({{"ast", tuple[keepProperty]}});
}

bool AstQuery::matchesExpectedType(Model::Node* node, Model::Node::SymbolType symbolType,
											  const QString& expectedType, const QStringList& args)
{
	using namespace OOInteraction;
	using SymbolType = Model::Node::SymbolType;
	if (SymbolType::VARIABLE == symbolType)
	{
		if (auto varDecl = DCast<OOModel::VariableDeclaration>(node))
			return StringComponents::stringForNode(varDecl->typeExpression()) == expectedType;
	}
	else
	{
		if (auto methodDecl = DCast<OOModel::Method>(node))
		{
			if (args.size() != methodDecl->arguments()->size()) return false;
			// TODO: here we support only one return value:
			Q_ASSERT(methodDecl->results()->size() <= 1);
			if ((methodDecl->results()->size() == 0 && (expectedType.isEmpty() || expectedType == "void"))
				 || (methodDecl->results()->size() > 0 &&
					  StringComponents::stringForNode(methodDecl->results()->at(0)->typeExpression()) == expectedType)
				 || expectedType == "_")
			{
				// return type matches check arguments:
				bool matches = true;
				for (int i = 0; i < args.size() && matches; ++i)
				{
					if (args[i] == "_") continue;
					matches = StringComponents::stringForNode(methodDecl->arguments()->at(i)->typeExpression()) == args[i];
				}
				return matches;
			}
		}
	}
	return false;
}

}
