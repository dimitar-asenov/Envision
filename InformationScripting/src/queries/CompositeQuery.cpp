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

namespace InformationScripting {

CompositeQuery::~CompositeQuery()
{
	SAFE_DELETE(inNode_);
	for (auto node : nodes_) SAFE_DELETE(node);
	// Don't delete the outNode_ as that we delete all Graphs which we returned in the execute methods.
}

QList<Optional<TupleSet> > CompositeQuery::execute(QList<TupleSet> input)
{
	for (const auto& ts: input)
		inNode_->calculatedOutputs_.push_back(ts);
	// Nodes for which we have all dependencies calculated:
	QQueue<QueryNode*> justExecutedQueries;
	justExecutedQueries.enqueue(inNode_);
	QSet<QueryNode*> fullyProcessedQueries;

	// Check if we have nodes which take no input:
	for (auto queryNode : nodes_)
	{
		if (queryNode->inputMap_.empty())
		{
			Q_ASSERT(queryNode->q_);
			queryNode->execute();
			justExecutedQueries.enqueue(queryNode);
		}
	}

	while (!justExecutedQueries.empty())
	{
		auto currentNode = justExecutedQueries.dequeue();
		// TODO: once we allow cyclic dependencies we have to be smarter here:
		Q_ASSERT (!fullyProcessedQueries.contains(currentNode));
		fullyProcessedQueries.insert(currentNode);

		for (int outIndex = 0; outIndex < currentNode->outputMap_.size(); ++outIndex)
		{
			auto outputMapping = currentNode->outputMap_[outIndex];

			bool hasOutput = outIndex < currentNode->calculatedOutputs_.size();

			for (auto receiverIt = outputMapping.begin(); receiverIt != outputMapping.end(); ++receiverIt)
			{
				auto receiver = *receiverIt;
				// find the input mapping of the receiver:
				auto inputIt = std::find_if(receiver->inputMap_.begin(), receiver->inputMap_.end(),
					[currentNode, outIndex] (const InputMapping& m) {
					return m.outputFrom_ == currentNode && m.outputIndex_ == outIndex;
				});
				Q_ASSERT(inputIt != receiver->inputMap_.end());
				if (hasOutput)
				{
					// NOTE if we want to execute in parallel then we probably shouldn't just abort.
					// early abort in case of error:
					if (!currentNode->calculatedOutputs_[outIndex])
						return {currentNode->calculatedOutputs_[outIndex]};
					receiver->addCalculatedInput(std::distance(receiver->inputMap_.begin(), inputIt),
														  currentNode->calculatedOutputs_[outIndex]);
				}
				else
					receiver->addCalculatedInput(std::distance(receiver->inputMap_.begin(), inputIt), TupleSet());


				if (receiver->canExecute())
				{
					receiver->execute();
					justExecutedQueries.enqueue(receiver);
				}
			}
		}
	}
	return outNode_->calculatedOutputs_;
}

void CompositeQuery::connectQuery(Query* from, Query* to)
{
	connectQuery(from, 0, to, 0);
}

void CompositeQuery::connectQuery(Query* from, int outIndex, Query* to, int inIndex)
{
	auto fromNode = nodeForQuery(from);
	auto toNode = nodeForQuery(to);

	addOutputMapping(fromNode, outIndex, toNode);
	addInputMapping(fromNode, outIndex, toNode, inIndex);
}

void CompositeQuery::connectInput(int inputIndex, Query* to, int atInput)
{
	auto toNode = nodeForQuery(to);

	addOutputMapping(inNode_, inputIndex, toNode);
	addInputMapping(inNode_, inputIndex, toNode, atInput);
}

void CompositeQuery::connectToOutput(Query* from, int outIndex)
{
	auto fromNode = nodeForQuery(from);

	addOutputMapping(fromNode, 0, outNode_);
	addInputMapping(fromNode, 0, outNode_, outIndex);
}

void CompositeQuery::connectToOutput(const QList<Query*>& outQueries)
{
	for (int i = 0; i < outQueries.size(); ++i)
		connectToOutput(outQueries[i], i);
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
	if (outIndex >= outNode->outputMap_.size()) outNode->outputMap_.resize(outIndex + 1);
	outNode->outputMap_[outIndex].insert(inNode);
}

void CompositeQuery::addInputMapping(CompositeQuery::QueryNode* outNode, int outIndex,
												 CompositeQuery::QueryNode* inNode, int inIndex)
{
	if (inIndex >= inNode->inputMap_.size()) inNode->inputMap_.resize(inIndex + 1);
	auto& existingMapping = inNode->inputMap_[inIndex];
	if (existingMapping.outputFrom_)
	{
		// Only single connection to input allowed:
		Q_ASSERT(existingMapping.outputFrom_ == outNode && existingMapping.outputIndex_ == outIndex);
		// Already there
		return;
	}
	existingMapping.outputFrom_ = outNode;
	existingMapping.outputIndex_ = outIndex;
}

CompositeQuery::QueryNode::~QueryNode()
{
	SAFE_DELETE(q_);
}

void CompositeQuery::QueryNode::addCalculatedInput(int index, Optional<TupleSet> g)
{
	// Fill non determined inputs with nullptrs:
	while (calculatedInputs_.size() - 1 < index)
		calculatedInputs_.push_back({"Not calculated yet"});
	// Insert current input at correct location
	calculatedInputs_[index] = g;
	// Set the inserted flag
	Q_ASSERT(index < inputMap_.size() && inputMap_[index].outputFrom_);
	inputMap_[index].inserted_ = true;
}

bool CompositeQuery::QueryNode::canExecute() const
{
	return std::all_of(inputMap_.begin(), inputMap_.end(), [](const InputMapping& m) {return m.inserted_;});
}

void CompositeQuery::QueryNode::execute()
{
	if (q_)
	{
		QList<TupleSet> input;
		QStringList allWarnings;
		for (const auto& opt : calculatedInputs_)
		{
			Q_ASSERT(opt);
			input.push_back(opt.value());
			if (opt.hasWarnings()) allWarnings << opt.warnings();
		}
		calculatedOutputs_ = q_->execute(input);
		for (auto& opt : calculatedOutputs_) opt.addWarnings(allWarnings);
	}
	else
		calculatedOutputs_ = calculatedInputs_;
}

} /* namespace InformationScripting */
