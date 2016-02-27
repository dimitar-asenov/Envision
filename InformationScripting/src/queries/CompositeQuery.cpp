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
	SAFE_DELETE(outNode_);
}

QList<Optional<TupleSet> > CompositeQuery::execute(QList<TupleSet> input)
{
	for (const auto& ts: input)
		inNode_->calculatedOutputs_.push_back(ts);
	// Nodes for which we have all dependencies calculated:
	QQueue<QueryNodeInCompositeQuery*> justExecutedQueries;
	justExecutedQueries.enqueue(inNode_);
	QSet<QueryNodeInCompositeQuery*> fullyProcessedQueries;

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
				while (inputIt != receiver->inputMap_.end())
				{
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
						receiver->addCalculatedInput(std::distance(receiver->inputMap_.begin(), inputIt), TupleSet{});

					// It could be that the receiver expects our output on multiple inputs, thus search on:
					inputIt = std::find_if(inputIt + 1, receiver->inputMap_.end(),
										[currentNode, outIndex] (const InputMapping& m) {
										return m.outputFrom_ == currentNode && m.outputIndex_ == outIndex;
					});
				}


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

Query* CompositeQuery::addQuery(std::unique_ptr<Query>&& query)
{
	auto rawQ = query.get();
	// A query should only be added once:
	Q_ASSERT(std::find_if(nodes_.begin(), nodes_.end(), [rawQ](QueryNodeInCompositeQuery* existing)
	{return existing->q_.get() == rawQ;})
					== nodes_.end());
	auto newNode = new QueryNodeInCompositeQuery{std::forward<std::unique_ptr<Query>>(query)};
	nodes_.push_back(newNode);
	return rawQ;
}

void CompositeQuery::connectQuery(Query* from, Query* to)
{
	connectQuery(from, 0, to, 0);
}

void CompositeQuery::connectQuery(Query* from, int outIndex, Query* to, int inIndex)
{
	auto fromNode = nodeForQuery(from);
	auto toNode = nodeForQuery(to);

	to->setHasInput();

	addOutputMapping(fromNode, outIndex, toNode);
	addInputMapping(fromNode, outIndex, toNode, inIndex);
}

void CompositeQuery::connectInput(int inputIndex, Query* to, int atInput)
{
	auto toNode = nodeForQuery(to);

	addOutputMapping(inNode_, inputIndex, toNode);
	addInputMapping(inNode_, inputIndex, toNode, atInput);
}

void CompositeQuery::connectToOutput(int queryOutIndex, Query* from, int compositeOutIndex)
{
	auto fromNode = nodeForQuery(from);

	addOutputMapping(fromNode, queryOutIndex, outNode_);
	addInputMapping(fromNode, queryOutIndex, outNode_, compositeOutIndex);
}

void CompositeQuery::setHasInput()
{
	Query::setHasInput();
	for (const auto& outChannel : inNode_->outputMap_)
		for (auto queryNode : outChannel)
			if (auto q = queryNode->q_.get())
				q->setHasInput();
}

QueryNodeInCompositeQuery* CompositeQuery::nodeForQuery(Query* q)
{
	auto it = std::find_if(nodes_.begin(), nodes_.end(), [q](QueryNodeInCompositeQuery* existing)
		{return existing->q_.get() == q;});
	// A query should be added first with addQuery():
	Q_ASSERT(it != nodes_.end());
	return *it;
}

void CompositeQuery::addOutputMapping(QueryNodeInCompositeQuery* outNode, int outIndex,
												  QueryNodeInCompositeQuery* inNode)
{
	if (outIndex >= outNode->outputMap_.size()) outNode->outputMap_.resize(outIndex + 1);
	outNode->outputMap_[outIndex].insert(inNode);
}

void CompositeQuery::addInputMapping(QueryNodeInCompositeQuery* outNode, int outIndex,
												 QueryNodeInCompositeQuery* inNode, int inIndex)
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

void QueryNodeInCompositeQuery::addCalculatedInput(int index, Optional<TupleSet> g)
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

bool QueryNodeInCompositeQuery::canExecute() const
{
	return std::all_of(inputMap_.begin(), inputMap_.end(), [](const InputMapping& m) {return m.inserted_;});
}

void QueryNodeInCompositeQuery::execute()
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

}
