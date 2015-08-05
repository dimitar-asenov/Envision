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

#pragma once

#include "../informationscripting_api.h"

namespace InformationScripting {

class InformationEdge;
class InformationNode;

class Graph
{
	public:
		~Graph();
		/**
		 * Each information source can register 1 NodeHash function. The function takes a node as input,
		 * and should return a pair of the hash value and a bool if the function could succesfully hash the value.
		 *
		 * The second parameter is useful as, e.g. a node from the tag information source can not be hashed by the
		 * ast information source.
		 */
		using NodeHash = std::function<QPair<std::size_t, bool>(const InformationNode*)>;
		using NodeCondition = std::function<bool(const InformationNode*)>;
		using EdgeCondition = std::function<bool(const InformationEdge*)>;

		static void registerNodeHash(NodeHash check);

		/**
		 * Adds the \a node to the graph. Only the returned node should be used after this method is called.
		 * (It might be that the graph already contains the same node, thus the properties will be merged)
		 *
		 * Note: Takes ownership on \a node.
		 */
		InformationNode* add(InformationNode* node);

		/**
		 * Creates a directed Edge from \a from to \a to with the name \a name.
		 *
		 * Returns the directed connection between \a from and \a to.
		 *
		 * If there is already a directed edge with the same \a name between \a from and \a to,
		 * the existing edge is returned with the count property increased.
		 *
		 * Note: The graph owns the returned edge.
		 */
		InformationEdge* addDirectedEdge(InformationNode* from, InformationNode* to, const QString& name);

		/**
		 * Connects \a a to \a b with an indirected edge with the name \a name.
		 *
		 * Returns the undirected connection between \a a and \a b.
		 *
		 * If there is already a undirected edge with the same \a name between \a a and \a b,
		 * the existing edge is returned with the count property increased.
		 *
		 * Note: The graph owns the returned edge.
		 */
		InformationEdge* addEdge(InformationNode* a, InformationNode* b, const QString& name);


		/**
		 * Removes the \a node and all edges to it.
		 */
		void remove(InformationNode* node);

		/**
		 * Removes all nodes in the list \a nodes and all edges to the nodes.
		 */
		void remove(QList<InformationNode*> nodes);

		QList<InformationNode*> nodes() const;

		/**
		 * Returns all nodes for which the NodeCondition \a holds holds.
		 */
		QList<InformationNode*> nodesForWhich(NodeCondition holds) const;

		/**
		 * Returns all edges for which the EdgeCondition \a holds holds.
		 */
		QList<InformationEdge*> edgesFowWhich(EdgeCondition holds) const;

	private:
		static QList<NodeHash> nodeHashFunctions_;

		QHash<std::size_t, InformationNode*> nodes_;
		QList<InformationEdge*> edges_;

		std::size_t hashValueOf(const InformationNode* n) const;
		/**
		 * Checks if there is a node in the graph with the same hash value as \a n,
		 * if there is the pointer to this node is returned, otherwise a null pointer is returned.
		 */
		InformationNode* findNode(const InformationNode* n) const;
		/**
		 * Merges the information of 2 nodes into the node \a into.
		 */
		void mergeNodes(InformationNode* into, InformationNode* from);

};

inline void Graph::registerNodeHash(Graph::NodeHash check) { nodeHashFunctions_ << check; }

} /* namespace InformationScripting */
