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

#include "../visitors/NodeGetter.h"
#include "QueryRegistry.h"

namespace InformationScripting {

const QStringList AstQuery::NODETYPE_ARGUMENT_NAMES{"t", "type"};
const QStringList AstQuery::NAME_ARGUMENT_NAMES{"n", "name"};
const QStringList AstQuery::ADD_AS_NAMES{"a", "addAs"};
const QStringList AstQuery::ATTRIBUTE_NAME_NAMES{"at", "attribute"};

AstQuery::AstQuery(ExecuteFunction exec, Model::Node* target, QStringList args,
						 std::vector<ArgumentRule> argumentRules)
	: target_{target}, arguments_{new ArgumentParser({
		 {NODETYPE_ARGUMENT_NAMES, "AST Type argument", NODETYPE_ARGUMENT_NAMES[1]},
		 {NAME_ARGUMENT_NAMES, "Name of a symbol", NAME_ARGUMENT_NAMES[1]},
		 {ADD_AS_NAMES, "Add as relation or nodes", ADD_AS_NAMES[1], "relation"},
		 {ATTRIBUTE_NAME_NAMES, "Attribute to search from", ATTRIBUTE_NAME_NAMES[1]}
		}, QStringList("AstQuery") + args)}, exec_{exec}
{
	for (const auto& rule : argumentRules)
		arguments_->checkRule(rule);
}

Optional<TupleSet> AstQuery::executeLinear(TupleSet input)
{
	return exec_(this, input);
}

void AstQuery::registerDefaultQueries()
{
	registerQuery("classes", &AstQuery::genericQuery, {}, "Class");
	registerQuery("methods", &AstQuery::genericQuery, {}, "Method");
	registerQuery("toClass", &AstQuery::toParentType, {}, "Class");
	registerQuery("bases", &AstQuery::baseClassesQuery);
	registerQuery("callgraph", &AstQuery::callGraph);
	registerQuery("ast", &AstQuery::genericQuery,
		{{ArgumentRule::requireOneOf, "ast requires either name or type argument",
		  {{NODETYPE_ARGUMENT_NAMES[1]}, {NAME_ARGUMENT_NAMES[1]}}}});
	registerQuery("toParent", &AstQuery::toParentType,
		{{ArgumentRule::requireAll, "toParent requires type argument", {{NODETYPE_ARGUMENT_NAMES[1]}}}});
	registerQuery("uses", &AstQuery::usesQuery,
		{{ArgumentRule::requireOneOf, "uses requires either name or type argument",
		  {{NODETYPE_ARGUMENT_NAMES[1]}, {NAME_ARGUMENT_NAMES[1]}}}});
	registerQuery("type", &AstQuery::typeFilter,
		{{ArgumentRule::requireAll, "typeFilter requires type argument", {{NODETYPE_ARGUMENT_NAMES[1]}}}});
	registerQuery("attribute", &AstQuery::attribute,
		{{ArgumentRule::requireAll, "attribute requires attribute argument", {{ATTRIBUTE_NAME_NAMES[1]}}}});
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
		args.append(QString("-%1=%2").arg(NODETYPE_ARGUMENT_NAMES[0], type));
}

Optional<TupleSet> AstQuery::baseClassesQuery(TupleSet)
{
	// TODO handle input
	TupleSet ts;
	if (arguments_->scope() == ArgumentParser::Scope::Local)
	{
		OOModel::Class* parentClass = DCast<OOModel::Class>(target_);
		if (!parentClass) parentClass = target_->firstAncestorOfType<OOModel::Class>();

		NamedProperty namedClass{"childClass", parentClass};
		ts.add({namedClass});

		addBaseEdgesFor(parentClass, namedClass, ts);

		adaptOutputForRelation(ts, "base class", {"baseClass"});
	}
	else if (arguments_->scope() == ArgumentParser::Scope::Global)
	{
		// TODO
	}
	return ts;
}

Optional<TupleSet> AstQuery::toParentType(TupleSet input)
{
	QString type = arguments_->argument(NODETYPE_ARGUMENT_NAMES[0]);
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
		ts.add({{"parentOfType", {}}, {"childNode", astNode}, {"parentNode", parentNode}});
	}
	adaptOutputForRelation(ts, "parentOfType", {"parentNode"});
	return ts;
}

Optional<TupleSet> AstQuery::callGraph(TupleSet input)
{
	TupleSet result;

	auto addCallgraphFor = [&result](std::vector<OOModel::Method*> methods)
	{
		for (auto method : methods)
		{
			Q_ASSERT(method);
			QSet<OOModel::Method*> seenMethods{method};
			auto callees = method->callees().toList();
			addCallInformation(result, method, callees);
			while (!callees.empty())
			{
				auto currentMethod = callees.takeLast();
				if (seenMethods.contains(currentMethod)) continue;
				seenMethods.insert(currentMethod);
				auto newCallees = currentMethod->callees().toList();
				addCallInformation(result, currentMethod, newCallees);
				callees << newCallees;
			}
		}
	};

	if (arguments_->scope() == ArgumentParser::Scope::Local)
	{
		if (auto method = DCast<OOModel::Method>(target_)) addCallgraphFor({method});
		else return {"Callgraph does only work on method nodes"};
	}
	else if (arguments_->scope() == ArgumentParser::Scope::Global)
		return {"Callgraph does not work globally"};
	else if (arguments_->scope() == ArgumentParser::Scope::Input)
	{
		// Keep input nodes
		result = input;
		std::vector<OOModel::Method*> methodsInInput;
		for (const auto& astTuple : input.tuples("ast"))
		{
			Model::Node* astNode = astTuple["ast"];
			if (auto method = DCast<OOModel::Method>(astNode)) methodsInInput.push_back(method);
		}
		if (methodsInInput.size() == 0) return {result, "Called callgraph without methods in input"};
		else addCallgraphFor(std::move(methodsInInput));
	}
	adaptOutputForRelation(result, "calls", {"caller", "callee"});
	return result;
}

Optional<TupleSet> AstQuery::genericQuery(TupleSet input)
{
	QString typeArgument = arguments_->argument(NODETYPE_ARGUMENT_NAMES[0]);
	QString nameArgument = arguments_->argument(NAME_ARGUMENT_NAMES[0]);
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

	if (arguments_->scope() == ArgumentParser::Scope::Local)
		addNodesOfType(tuples, matcher, target_);
	else if (arguments_->scope() == ArgumentParser::Scope::Global)
		addNodesOfType(tuples, matcher);
	else if (arguments_->scope() == ArgumentParser::Scope::Input)
	{
		tuples = input;

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple) addNodesOfType(tuples, matcher, t["ast"]);
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

	if (arguments_->scope() == ArgumentParser::Scope::Local)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1, target_, suggestable);
	else if (arguments_->scope() == ArgumentParser::Scope::Global)
		matchingNodes = Model::NameResolver::mostLikelyMatches(name, -1, nullptr, suggestable);
	else if (arguments_->scope() == ArgumentParser::Scope::Input)
	{
		tuples = input;

		// Note here we remove the input nodes.
		auto tuple = tuples.take("ast");
		for (const auto& t : tuple)
			matchingNodes << Model::NameResolver::mostLikelyMatches(name, -1, t["ast"], suggestable);
	}
	// If we have a type argument filter the results:
	const QString type = arguments_->argument(NODETYPE_ARGUMENT_NAMES[0]);
	Model::SymbolMatcher matcher = Model::SymbolMatcher::guessMatcher(type);
	for (auto matchingNode : matchingNodes)
		if (type.isEmpty() || matcher.matches(matchingNode.second->typeName()))
			tuples.add({{"ast", matchingNode.second}});

	return tuples;
}

Optional<TupleSet> AstQuery::usesQuery(TupleSet input)
{
	TupleSet result;
	QHash<Model::Node*, QList<Model::Reference*>> references;

	if (arguments_->scope() == ArgumentParser::Scope::Local)
		references[target_] = NodeGetter::allNodesOfType<Model::Reference>(target_);
	else if (arguments_->scope() == ArgumentParser::Scope::Global)
		references[target_->root()] = NodeGetter::allNodesOfType<Model::Reference>(target_->root());
	else if (arguments_->scope() == ArgumentParser::Scope::Input)
	{
		auto tuples = input;

		auto tuple = tuples.tuples("ast");
		for (const auto& t : tuple) references[t["ast"]] = NodeGetter::allNodesOfType<Model::Reference>(t["ast"]);

		// Keep all non ast nodes
		result = tuples;
	}

	auto typeMatcher = Model::SymbolMatcher::guessMatcher(arguments_->argument(NODETYPE_ARGUMENT_NAMES[0]));
	auto nameMatcher = Model::SymbolMatcher::guessMatcher(arguments_->argument(NAME_ARGUMENT_NAMES[0]));

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
	auto typeArgument = arguments_->argument(NODETYPE_ARGUMENT_NAMES[1]);
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

	// Lambda which returns true for all nodes which match the expected type.
	auto keepNode = [expectedSymbolType, expectedType, arguments](Model::Node* node) {
		auto symbolTypes = node->symbolType();

		if (symbolTypes.testFlag(expectedSymbolType))
			return matchesExpectedType(node, expectedSymbolType, expectedType, arguments);
		return false;
	};

	if (arguments_->scope() == ArgumentParser::Scope::Local)
		addNodesForWhich(result, keepNode, target_);
	else if (arguments_->scope() == ArgumentParser::Scope::Global)
		addNodesForWhich(result, keepNode);
	else if (arguments_->scope() == ArgumentParser::Scope::Input)
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
	const QString attributeName = arguments_->argument(ATTRIBUTE_NAME_NAMES[1]);
	Q_ASSERT(!attributeName.isEmpty());

	std::vector<Model::Node*> foundAttributeNodes;
	auto findAttribute = [&attributeName, &foundAttributeNodes](Model::Node* node) {
		if (auto compositeNode = DCast<Model::CompositeNode>(node))
			if (compositeNode->hasAttribute(attributeName))
				foundAttributeNodes.push_back(compositeNode->get(attributeName));
	};

	TupleSet result;
	if (arguments_->scope() == ArgumentParser::Scope::Local)
	{
		findAttribute(target_);
	}
	else if (arguments_->scope() == ArgumentParser::Scope::Global)
	{
		auto nodesWithAttribute = NodeGetter::allNodesWhich(target_->root(), [&attributeName](Model::Node* node) {
			if (auto composite = DCast<Model::CompositeNode>(node))
				return composite->hasAttribute(attributeName);
			return false;
		});
		for (auto node : nodesWithAttribute)
			findAttribute(node);
	}
	else if (arguments_->scope() == ArgumentParser::Scope::Input)
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
	auto allNodeOfType =  NodeGetter::allNodesWhich(from, [matcher](Model::Node* node)
																			{return matcher.matches(node->typeName());});
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

template <class Predicate>
void AstQuery::addNodesForWhich(TupleSet& ts, Predicate holds, Model::Node* from)
{
	if (!from) from = target_->root();
	auto allNodeOfType =  NodeGetter::allNodesWhich(from, holds);
	for (auto node : allNodeOfType)
		ts.add({{"ast", node}});
}

void AstQuery::adaptOutputForRelation(TupleSet& tupleSet, const QString& relationName,
												  const QStringList& keepProperties)
{
	if (arguments_->argument(ADD_AS_NAMES[0]) != "relation")
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

void AstQuery::registerQuery(const QString& name, AstQuery::ExecuteFunction methodToCall,
									  std::vector<ArgumentRule> argumentRules, const QString& setTypeTo)
{
	QueryRegistry::instance().registerQueryConstructor(name,
		[methodToCall, setTypeTo, argumentRules](Model::Node* target, QStringList args) {
			if (!setTypeTo.isNull()) AstQuery::setTypeTo(args, setTypeTo);
			return new AstQuery(methodToCall, target, args, argumentRules);
	});
}

} /* namespace InformationScripting */
