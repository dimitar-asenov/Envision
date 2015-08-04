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

#include "Query.h"

namespace InformationScripting {

class INFORMATIONSCRIPTING_API CompositeQuery : public Query
{
	public:
		virtual ~CompositeQuery() override;

		virtual QList<Graph*> execute(QList<Graph*> input) override;

		/**
		 * Adds the Query \a q to this CompositeQuery. This query takes ownership of \a q.
		 */
		void addQuery(Query* q);

		/**
		 * Connects output 0 from Query \a from to input 0 of Query \a to.
		 *
		 * If either \a from or \a to was not yet inserted they will be and this query will take ownership of it.
		 */
		void connectQuery(Query* from, Query* to);

		void connectQuery(Query* from, int outIndex, Query* to, int inIndex);

		void connectToOutput(Query* from, int outIndex = 0);


	private:
		struct QueryNode;
		/**
		 * Describes an input mapping:
		 * The output with index \a outputIndex_ from the Query \a outputFrom_
		 * is mapped to the input with index \a inputIndex_.
		 *
		 * Note: 1 Input can only receive a single output.
		 */
		struct InputMapping {
				int inputIndex_{};
				QueryNode* outputFrom_{};
				int outputIndex_{};
				// Indicates whether this output has been calculated and set.
				bool inserted_{};
		};

		/**
		 * Decribes an output mapping:
		 * 1 Output can go to multiple receivers.
		 */
		struct OutputMapping {
				int outputIndex_{};
				QSet<QueryNode*> outputReceivers_{};
		};

		struct QueryNode {
				QueryNode(Query* q) : q_{q} {}
				~QueryNode();
				QList<InputMapping> inputs_;
				QList<OutputMapping> outputs_;

				QList<Graph*> calculatedInputs_;
				QList<Graph*> calculatedOutputs_;

				void addCalculatedInput(int index, Graph* g);
				bool canExecute() const;
				void execute();

				Query* q_{};
		};
		// Pseudo node to connect, to get input from execute method
		QueryNode* inNode_{new QueryNode(nullptr)};
		// Pseudo node to connect, to map the output
		QueryNode* outNode_{new QueryNode(nullptr)};
		QList<QueryNode*> nodes_;

		QueryNode* nodeForQuery(Query* q);

		void addOutputMapping(QueryNode* outNode, int outIndex, QueryNode* inNode);
		void addInputMapping(QueryNode* outNode, int outIndex, QueryNode* inNode, int inIndex);
};

} /* namespace InformationScripting */
