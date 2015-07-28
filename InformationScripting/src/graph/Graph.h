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
		// TODO instead of IsEqual we should use hashing in the future, see github discussion.
		/**
		 * IsEqual takes 2 nodes and returns if they are the same for some data source.
		 */
		using IsEqual = std::function<bool(const InformationNode*, const InformationNode*)>;
		using NodeCondition = std::function<bool(const InformationNode*)>;
		using EdgeCondition = std::function<bool(const InformationEdge*)>;

		/**
		 * Registers the IsEqual \a check.
		 */
		static inline void addEqualityCheck(IsEqual check);

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


		/**
		 * Returns all nodes for which the NodeCondition \a holds holds.
		 */
		QList<InformationNode*> nodesForWhich(NodeCondition holds) const;

		/**
		 * Returns all edges for which the EdgeCondition \a holds holds.
		 */
		QList<InformationNode*> edgesFowWhich(EdgeCondition holds) const;

	private:
		static QList<IsEqual> equalityChecks_;

		QList<InformationNode*> nodes_;
		QList<InformationEdge*> edges_;

};

void Graph::addEqualityCheck(Graph::IsEqual check) { equalityChecks_ << check; }

} /* namespace InformationScripting */
