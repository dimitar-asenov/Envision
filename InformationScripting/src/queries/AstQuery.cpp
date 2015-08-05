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

#include "../graph/Graph.h"
#include "../graph/InformationNode.h"

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

QList<Graph*> AstQuery::execute(QList<Graph*> input)
{
	QList<Graph*> result;
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
	}
	// Clean unhandled input:
	for (auto& g : input) SAFE_DELETE(g);

	return result;
}

Graph* AstQuery::classesQuery(QList<Graph*>&)
{
	auto g = new Graph();
	if (scope_ == Scope::Local)
	{
		auto parentProject = target_->firstAncestorOfType<OOModel::Project>();
		for (auto childClass : *parentProject->classes())
		{
			auto node = new InformationNode({{"ast", childClass}});
			g->add(node);
		}
	}
	else if (scope_ == Scope::Global)
	{
		addGlobalNodesOfType<OOModel::Class>(g);
	}
	else if (scope_ == Scope::Input)
	{
		// TODO
	}
	return g;
}

Graph* AstQuery::methodsQuery(QList<Graph*>& input)
{
	if (scope_ == Scope::Local)
	{
		auto g = new Graph();
		auto parentClass = target_->firstAncestorOfType<OOModel::Class>();
		for (auto method : *parentClass->methods())
		{
			auto node = new InformationNode({{"ast", method}});
			g->add(node);
		}
		return g;
	}
	else if (scope_ == Scope::Global)
	{
		auto g = new Graph();
		addGlobalNodesOfType<OOModel::Method>(g);
		return g;
	}
	else if (scope_ == Scope::Input)
	{
		Q_ASSERT(input.size());
		auto g = input.takeFirst();

		auto canContainMethod = [](const InformationNode* n) {
			if (n->contains("ast")) {
				Model::Node* astNode = (*n)["ast"];
				return DCast<OOModel::Declaration>(astNode) != nullptr;
			}
			return false;
		};

		QList<InformationNode*> nodes = g->nodes(canContainMethod);
		for (auto node : nodes)
		{
			Model::Node* astNode = (*node)["ast"];
			AllNodesOfType<OOModel::Method> visitor;
			visitor.visit(astNode);
			auto methods = visitor.results();
			// TODO for now we just remove the input node, this might not always be what we want
			g->remove(node);
			for (auto method : methods)
				g->add(new InformationNode({{"ast", method}}));
		}
		return g;
	}
	Q_ASSERT(false);
	return nullptr;
}

Graph* AstQuery::baseClassesQuery(QList<Graph*>&)
{
	// TODO handle input
	auto g = new Graph();
	if (scope_ == Scope::Local)
	{
		OOModel::Class* parentClass = DCast<OOModel::Class>(target_);
		if (!parentClass) parentClass = target_->firstAncestorOfType<OOModel::Class>();

		auto classNode = new InformationNode({{"ast", parentClass}});
		classNode = g->add(classNode);

		addBaseEdgesFor(parentClass, classNode, g);
	}
	else if (scope_ == Scope::Global)
	{
		// TODO
	}
	return g;
}

Graph* AstQuery::toClassNode(QList<Graph*>& input)
{
	Q_ASSERT(input.size());
	auto g = input.takeFirst();
	auto canBeInClass = [](const InformationNode* node) {
		node->contains("ast");
		Model::Node* n = (*node)["ast"];
		return n->firstAncestorOfType<OOModel::Class>() != nullptr;
	};

	auto nodes = g->nodes(canBeInClass);
	QList<OOModel::Class*> classes;
	for (auto node : nodes)
	{
		Model::Node* astNode = (*node)["ast"];
		auto classParent = astNode->firstAncestorOfType<OOModel::Class>();
		if (!classes.contains(classParent)) classes.push_back(classParent);
		// TODO currently we remove all the converted nodes, this also means we lose all connections:
		g->remove(node);
	}
	for (auto foundClass : classes)
		g->add(new InformationNode({{"ast", foundClass}}));
	return g;
}

Graph* AstQuery::callGraph(QList<Graph*>&)
{
	auto g = new Graph();
	if (scope_ == Scope::Local)
	{
		auto methodTarget = target_->firstAncestorOfType<OOModel::Method>();
		Q_ASSERT(methodTarget);
		QSet<OOModel::Method*> seenMethods{methodTarget};
		auto methods = methodTarget->callees().toList();
		addCallInformation(g, methodTarget, methods);
		while (!methods.empty())
		{
			auto currentMethod = methods.takeLast();
			if (seenMethods.contains(currentMethod)) continue;
			seenMethods.insert(currentMethod);
			auto newCallees = currentMethod->callees().toList();
			addCallInformation(g, currentMethod, newCallees);
			methods << newCallees;
		}
	}
	// TODO handle other cases, global propably doesn't make sense.
	return g;
}

void AstQuery::addBaseEdgesFor(OOModel::Class* childClass, InformationNode* classNode, Graph* g)
{
	auto bases = childClass->directBaseClasses();
	for (auto base : bases)
	{
		auto baseNode = new InformationNode({{"ast", base}});
		baseNode = g->add(baseNode);
		g->addDirectedEdge(classNode, baseNode, "base class");
		addBaseEdgesFor(base, baseNode, g);
	}
}

void AstQuery::addCallInformation(Graph* g, OOModel::Method* method, QList<OOModel::Method*> callees)
{
	auto methodNode = g->add(new InformationNode({{"ast", method}}));
	for (auto callee : callees)
	{
		auto calleeNode = g->add(new InformationNode({{"ast", callee}}));
		g->addDirectedEdge(methodNode, calleeNode, "calls");
	}
}

template<class NodeType>
void AstQuery::addGlobalNodesOfType(Graph* g)
{
	auto root = target_->manager()->root();
	AllNodesOfType<NodeType> visitor;
	visitor.visit(root);
	auto allNodeOfType =  visitor.results();
	for (auto node : allNodeOfType)
		g->add(new InformationNode({{"ast", node}}));
}

} /* namespace InformationScripting */
