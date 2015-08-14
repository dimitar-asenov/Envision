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

#include "Graph.h"

#include "InformationNode.h"

namespace InformationScripting {

QList<Graph::NodeHash> Graph::nodeHashFunctions_;

Graph::~Graph()
{
	for (auto& e : edges_) SAFE_DELETE(e);
	for (auto& n : nodes_) SAFE_DELETE(n);
}

Graph* Graph::clone()
{
	auto cloned = new Graph();
	QMap<InformationNode*, InformationNode*> clonedNodes;
	for (auto node : nodes_)
	{
		auto clonedNode = node->clone();
		auto inserted = cloned->add(clonedNode);
		Q_ASSERT(inserted == clonedNode);
		clonedNodes[node] = clonedNode;
		cloned->nodes_[hashOf(clonedNode)] = clonedNode;
	}
	for (auto edge: edges_)
	{
		auto clonedEdge = edge->clone();
		clonedEdge->setFrom(clonedNodes[edge->from()]);
		clonedEdge->setTo(clonedNodes[edge->to()]);
		cloned->edges_ << clonedEdge;
	}
	return cloned;
}

InformationNode* Graph::add(InformationNode* node)
{
	Q_ASSERT(!nodeHashFunctions_.empty());

	InformationNode* existingNode = findNode(node);

	if (existingNode && existingNode != node)
	{
		mergeNodes(existingNode, node);
		SAFE_DELETE(node);
	}
	else
	{
		nodes_[hashOf(node)] = node;
		existingNode = node;
	}
	return existingNode;
}

InformationEdge* Graph::addEdge(InformationNode* from, InformationNode* to, const QString& name,
										  InformationEdge::Orientation orientation, bool incrementCountIfExisting)
{
	// We only allow existing nodes:
	Q_ASSERT(from == findNode(from));
	Q_ASSERT(to == findNode(to));
	// TODO this might be a possible performance hit if we have many edges.
	for (auto edge : edges_)
	{
		// TODO if directed only this condition
		bool directedMatch = edge->from() == from && edge->to() == to;
		bool undirectedMatch = edge->from() == to && edge->to() == from;
		if (edge->name() == name && ((orientation == InformationEdge::Orientation::Directed && directedMatch)
							|| (orientation == InformationEdge::Orientation::Directed && (directedMatch || undirectedMatch))))
		{
			auto existingEdge = edge;
			Q_ASSERT(existingEdge->orientation() == orientation);
			if (incrementCountIfExisting) existingEdge->incrementCount();
			return existingEdge;
		}
	}
	auto edge = new InformationEdge(from, to, name, orientation);
	edges_.push_back(edge);
	return edge;
}

void Graph::remove(InformationNode* node)
{
	Q_ASSERT(node);
	auto hash = hashOf(node);
	auto nodeIt = nodes_.find(hash);
	// If the node isn't in the graph we don't have to do anything:
	if (nodeIt == nodes_.end()) return;

	// It could be that node points to a different node with the same hash.
	// TODO in this case we might also want to merge the infos?
	auto ownedNode = node;
	if (ownedNode != *nodeIt) ownedNode = *nodeIt;

	for (auto edgeIt = edges_.begin(); edgeIt != edges_.end();)
	{
		auto edge = *edgeIt;
		if (edge->from() == ownedNode || edge->to() == ownedNode)
		{
			SAFE_DELETE(edge);
			edgeIt = edges_.erase(edgeIt);
		}
		else
			++edgeIt;
	}

	SAFE_DELETE(ownedNode);
	nodes_.erase(nodeIt);
}

void Graph::remove(QList<InformationNode*> nodes)
{
	for (auto node : nodes) remove(node);
}

QList<InformationNode*> Graph::nodes(Graph::NodeCondition holds) const
{
	if (!holds) return nodes_.values();

	QList<InformationNode*> result;
	for (auto node : nodes_) if (holds(node)) result.push_back(node);
	return result;
}

QList<InformationEdge*> Graph::edges(Graph::EdgeCondition holds) const
{
	if (!holds) return edges_;

	QList<InformationEdge*> result;
	for (auto edge : edges_) if (holds(edge)) result.push_back(edge);
	return result;
}

std::size_t Graph::hashOf(const InformationNode* n) const
{
	for (auto hashFunction : nodeHashFunctions_)
	{
		auto result = hashFunction(n);
		if (result.second) return result.first;
	}
	// There should be at least one function registered for a certain node
	Q_ASSERT(false);
}

InformationNode*Graph::findNode(const InformationNode* n) const
{
	auto hash = hashOf(n);
	auto it = nodes_.find(hash);
	if (it != nodes_.end()) return it.value();
	return nullptr;
}

void Graph::mergeNodes(InformationNode* into, InformationNode* from)
{
	for (auto namedProperty : *from)
	{
		QString name = namedProperty.first;
		if (!into->contains(name))
			into->insert(name, namedProperty.second);
		else
			Q_ASSERT((*from)[name] == namedProperty.second);
	}
}

} /* namespace InformationScripting */
