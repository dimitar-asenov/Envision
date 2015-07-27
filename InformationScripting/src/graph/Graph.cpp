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
#include "InformationEdge.h"

namespace InformationScripting {

void Graph::add(InformationNode*& node)
{
	InformationNode* existingNode = nullptr;
	for (auto graphNode : nodes_)
	{
		for (auto checker : sameCheckers_)
		{
			if (checker(graphNode, node))
			{
				existingNode = graphNode;
				break;
			}
		}
		if (existingNode) break;
	}
	if (existingNode) node = existingNode;
	else nodes_.push_back(node);
}

InformationEdge* Graph::addDirectedEdge(InformationNode*, InformationNode*, const QString&)
{
	// TODO
	return nullptr;
}

InformationEdge* Graph::addEdge(InformationNode*, InformationNode*, const QString&)
{
	// TODO
	return nullptr;
}

void Graph::remove(InformationNode*)
{
	// TODO
}

void Graph::remove(QList<InformationNode*> nodes)
{
	for (auto node : nodes) remove(node);
}

QList<InformationNode*> Graph::nodesForWhich(Graph::NodeCondition holds) const
{
	Q_ASSERT(holds);

	QList<InformationNode*> result;
	for (auto node : nodes_) if (holds(node)) result.push_back(node);
	return result;
}

QList<InformationNode*> Graph::edgesFowWhich(Graph::EdgeCondition) const
{
	// TODO
	return {};
}

} /* namespace InformationScripting */
