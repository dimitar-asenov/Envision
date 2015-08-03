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

#include "CompositeQuery.h"

#include "../graph/Graph.h"

namespace InformationScripting {

CompositeQuery::~CompositeQuery()
{
	SAFE_DELETE(inNode_);
	for (auto node : nodes_) SAFE_DELETE(node);
	// Don't delete the outNode_ as that we delete all Graphs which we returned in the execute methods.
}

QList<Graph*> CompositeQuery::execute(QList<Graph*> input)
{
	inNode_->calculatedOutputs_ = input;
	// Nodes for which we have all dependencies calculated:
	QQueue<QueryNode*> workingSetNodes;
	workingSetNodes.enqueue(inNode_);
	QSet<QueryNode*> visitedNodes;

	// Check if we have nodes which take no input:
	for (auto queryNode : nodes_)
	{
		if (queryNode->inputs_.empty())
		{
			Q_ASSERT(queryNode->q_);
			queryNode->execute();
			workingSetNodes.enqueue(queryNode);
		}
	}

	while (!workingSetNodes.empty())
	{
		auto currentNode = workingSetNodes.dequeue();
		// TODO: once we allow cyclic dependencies we have to be smarter here:
		if (visitedNodes.contains(currentNode)) continue;
		visitedNodes.insert(currentNode);

		for (auto outputMapping : currentNode->outputs_)
		{
			int outIndex = outputMapping.outputIndex_;
			// TODO: this shouldn't be an assertion later, but rather some error which is presented to the user.
			Q_ASSERT(outIndex < currentNode->calculatedOutputs_.size());

			// FIXME: multiple output receivers requires Graph copy constructor:
			Q_ASSERT(outputMapping.outputReceivers_.size() <= 1);

			if (outputMapping.outputReceivers_.size() == 1)
			{
				auto receiver = *outputMapping.outputReceivers_.begin();
				// find the input mapping of the receiver:
				auto inputIt = std::find_if(receiver->inputs_.begin(), receiver->inputs_.end(),
					[currentNode, outIndex] (const InputMapping& m) {
					return m.outputFrom_ == currentNode && m.outputIndex_ == outIndex;
				});
				Q_ASSERT(inputIt != receiver->inputs_.end());
				auto inputMapping = *inputIt;
				receiver->addCalculatedInput(inputMapping.inputIndex_, currentNode->calculatedOutputs_[outIndex]);

				if (receiver->canExecute())
				{
					receiver->execute();
					workingSetNodes.enqueue(receiver);
				}
			}
		}
	}
	return outNode_->calculatedOutputs_;
}

void CompositeQuery::addQuery(Query* q)
{
	if (!nodeForQuery(q)) nodes_.push_back(new QueryNode(q));
}

void CompositeQuery::connectQuery(Query* from, Query* to)
{
	auto fromNode = nodeForQuery(from);
	auto toNode = nodeForQuery(to);

	addOutputMapping(fromNode, 0, toNode);
	addInputMapping(fromNode, 0, toNode, 0);
}

void CompositeQuery::connectToOutput(Query* from, int outIndex)
{
	auto fromNode = nodeForQuery(from);

	addOutputMapping(fromNode, 0, outNode_);
	addInputMapping(fromNode, 0, outNode_, outIndex);
}

CompositeQuery::QueryNode* CompositeQuery::nodeForQuery(Query* q)
{
	auto it = std::find_if(nodes_.begin(), nodes_.end(), [q](QueryNode* d) {return d->q_ == q;});
	if (it != nodes_.end()) return *it;
	auto newNode = new QueryNode(q);
	nodes_.push_back(newNode);
	return newNode;
}

void CompositeQuery::addOutputMapping(CompositeQuery::QueryNode* outNode, int outIndex,
												  CompositeQuery::QueryNode* inNode)
{
	// Add output mapping
	auto it = std::find_if(outNode->outputs_.begin(), outNode->outputs_.end(),
					 [outIndex](const OutputMapping& m) {return m.outputIndex_ == outIndex;});
	if (it != outNode->outputs_.end())
		it->outputReceivers_.insert(inNode);
	else
		outNode->outputs_.push_back({outIndex, {inNode}});
}

void CompositeQuery::addInputMapping(CompositeQuery::QueryNode* outNode, int outIndex,
												 CompositeQuery::QueryNode* inNode, int inIndex)
{
	// Add output mapping
	auto it = std::find_if(inNode->inputs_.begin(), inNode->inputs_.end(),
					 [inIndex](const InputMapping& m) {return m.inputIndex_ == inIndex;});
	if (it != inNode->inputs_.end())
	{
		// Only single connection to input allowed:
		Q_ASSERT(it->outputFrom_ == outNode && it->outputIndex_ == outIndex);
		// Already there
		return;
	}
	inNode->inputs_.push_back({inIndex, outNode, outIndex});
}

CompositeQuery::QueryNode::~QueryNode()
{
	// For all calculated outputs check if they are mapped to something, if not delete them:
	for (int i = 0; i < calculatedOutputs_.size(); ++i)
	{
		auto it = std::find_if(outputs_.begin(), outputs_.end(),
									  [i](const OutputMapping& m) {return m.outputIndex_ == i;});
		if (it == outputs_.end())
			SAFE_DELETE(calculatedOutputs_[i]);
	}
	SAFE_DELETE(q_);
}

void CompositeQuery::QueryNode::addCalculatedInput(int index, Graph* g)
{
	// Fill non determined inputs with nullptrs:
	while (calculatedInputs_.size() - 1 < index)
		calculatedInputs_.push_back(nullptr);
	// Insert current input at correct location
	calculatedInputs_[index] = g;
	// Set the inserted flag
	auto it = std::find_if(inputs_.begin(), inputs_.end(),
								  [index](const InputMapping& m) {return m.inputIndex_ == index;});
	Q_ASSERT(it != inputs_.end());
	it->inserted_ = true;
}

bool CompositeQuery::QueryNode::canExecute() const
{
	return std::all_of(inputs_.begin(), inputs_.end(), [](const InputMapping& m) {return m.inserted_;});
}

void CompositeQuery::QueryNode::execute()
{
	if (q_)
		calculatedOutputs_ = q_->execute(calculatedInputs_);
	else
		calculatedOutputs_ = calculatedInputs_;
}

} /* namespace InformationScripting */
