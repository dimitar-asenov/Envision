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

struct QueryNodeInCompositeQuery;
class Query;

struct InputMapping {
		QueryNodeInCompositeQuery* outputFrom_{};
		int outputIndex_{};
		// Indicates whether this output has been calculated and set.
		bool inserted_{};
};

struct QueryNodeInCompositeQuery {
		QueryNodeInCompositeQuery(std::unique_ptr<Query>&& q) : q_{std::forward<std::unique_ptr<Query>>(q)} {}

		/**
		 * Describes an input mapping:
		 * The output with index \a outputIndex_ from the Query \a outputFrom_
		 * is mapped to the input with index i in the vector.
		 *
		 * Note: 1 Input can only receive a single output.
		 */
		QVector<InputMapping> inputMap_;

		/**
		 * Decribes an output mapping:
		 * 1 Output can go to multiple receivers.
		 */
		QVector<QSet<QueryNodeInCompositeQuery*>> outputMap_;

		QList<Optional<TupleSet>> calculatedInputs_;
		QList<Optional<TupleSet>> calculatedOutputs_;

		void addCalculatedInput(int index, Optional<TupleSet> g);
		bool canExecute() const;
		void execute();

		std::unique_ptr<Query> q_{};
};

class INFORMATIONSCRIPTING_API CompositeQuery : public Query
{
	public:
		virtual ~CompositeQuery() override;

		virtual QList<Optional<TupleSet>> execute(QList<TupleSet> input) override;

		/**
		 * Adds the \a query to this CompositeQuery. This CompositeQuery will take ownership of \a query.
		 * The returned value is the underlying raw pointer in \a query which can be used to wire \a query with other
		 * queries.
		 */
		Query* addQuery(std::unique_ptr<Query>&& query);

		/**
		 * Connects output 0 from Query \a from to input 0 of Query \a to.
		 *
		 * If either \a from or \a to was not yet inserted they will be and this query will take ownership of it.
		 */
		void connectQuery(Query* from, Query* to);

		void connectQuery(Query* from, int outIndex, Query* to, int inIndex);

		void connectInput(int inputIndex, Query* to, int atInput = 0);
		void connectToOutput(int queryOutIndex, Query* from, int compositeOutIndex);

		int inputCount() const;
		int outputCount() const;

		virtual void setHasInput() override;

	private:
		// Pseudo node to connect, to get input from execute method
		QueryNodeInCompositeQuery* inNode_{new QueryNodeInCompositeQuery{nullptr}};
		// Pseudo node to connect, to map the output
		QueryNodeInCompositeQuery* outNode_{new QueryNodeInCompositeQuery{nullptr}};
		QList<QueryNodeInCompositeQuery*> nodes_;

		QueryNodeInCompositeQuery* nodeForQuery(Query* q);

		void addOutputMapping(QueryNodeInCompositeQuery* outNode, int outIndex, QueryNodeInCompositeQuery* inNode);
		void addInputMapping(QueryNodeInCompositeQuery* outNode, int outIndex,
									QueryNodeInCompositeQuery* inNode, int inIndex);
};

inline int CompositeQuery::inputCount() const { return std::max(1, inNode_->outputMap_.size()); }
inline int CompositeQuery::outputCount() const { return std::max(1, outNode_->inputMap_.size()); }

}
