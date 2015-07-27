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
		/**
		 * SameCheck takes 2 nodes and returns if they are the same for some data source.
		 */
		using SameCheck = std::function<bool(const InformationNode*, const InformationNode*)>;
		using NodeCondition = std::function<bool(const InformationNode*)>;
		using EdgeCondition = std::function<bool(const InformationEdge*)>;


		/**
		 * Registers the SameCheck \a check.
		 */
		inline void addSameCheck(SameCheck check);

		/**
		 * Adds the \a node to the graph. If the graph already contains the same node (checked by unique checkers),
		 * the node reference is overwritten with the existing node.
		 *
		 * Note: Takes ownership on \a node.
		 */
		void add(InformationNode*& node);

		/**
		 * Creates a directed Edge from \a from to \a to with the name \a name.
		 *
		 * Returns the directed connection between \a from and \a to.
		 *
		 * If there is already a directed edge with the same \a name between \a from and \a to,
		 * the existing edge is returned with the count property increased.
		 * If there is already a undirected edge with the same \a name between \a from and \a to,
		 * an exception is thrown.
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
		 * If there is already a directed edge with the same \a name from \a a to \a b, or from \a b to \a a,
		 * the existing edge is returned.
		 *
		 * Note: The graph owns the returned edge.
		 *
		 * >NOTE: It is nicer to just return a single edge rather than 2 edges, as it is an implementation detail
		 * an exception is thrown.
		 */
		InformationEdge* addEdge(InformationNode* a, InformationNode* b, const QString& name);


		/**
		 * Removes the \a node and all connections to it.
		 */
		void remove(InformationNode* node);

		/**
		 * Removes all nodes in the list \a nodes and all conntections to the nodes.
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
		QList<SameCheck> sameCheckers_;
		QList<InformationNode*> nodes_;
};

void Graph::addSameCheck(Graph::SameCheck check) { sameCheckers_ << check; }

} /* namespace InformationScripting */
