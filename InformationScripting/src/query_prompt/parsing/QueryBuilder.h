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

#include "../../informationscripting_api.h"

#include "ModelBase/src/visitor/Visitor.hpp"

namespace Model {
	class Node;
}

namespace InformationScripting {

class CommandNode;
class CompositeQuery;
class CompositeQueryNode;
class OperatorQueryNode;
class Query;
class QueryExecutor;

class INFORMATIONSCRIPTING_API QueryBuilder : public Model::Visitor<QueryBuilder, std::vector<std::unique_ptr<Query>>>
{
	public:
		QueryBuilder(Model::Node* target, QueryExecutor* executor);
		static void init();

	private:
		QueryExecutor* executor_{};
		Model::Node* target_{};

		static std::vector<std::unique_ptr<Query>> visitCommand(QueryBuilder* self, CommandNode* command);
		static std::vector<std::unique_ptr<Query>> visitList(QueryBuilder* self, CompositeQueryNode* list);
		static std::vector<std::unique_ptr<Query>> visitOperator(QueryBuilder* self, OperatorQueryNode* op);

		static void connectQueriesWith(CompositeQuery* composite, CompositeQuery* queries,
										Query* connectionQuery, Query* outputQuery = nullptr);

		/**
		 * Connects \a left with \a right queries in the \a composite, using a union operator specified in \a op.
		 *
		 * Note that \a left and \a right have to be previously added to \a composite.
		 */
		static void connectAsUnion(CompositeQuery* composite, CompositeQuery* left, Query* right, OperatorQueryNode* op);

		/**
		 * Connects \a left with \a right queries in the \a composite, building a split operation.
		 *
		 * Note that \a left and \a right have to be previously added to \a composite.
		 */
		static void connectAsSplit(CompositeQuery* composite, Query* left, CompositeQuery* right);
};

}
