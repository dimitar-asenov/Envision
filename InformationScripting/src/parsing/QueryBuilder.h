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

namespace Model {
	class Node;
}

namespace InformationScripting {

class CompositeQuery;
class Query;

class INFORMATIONSCRIPTING_API QueryBuilder
{
	public:
		static QueryBuilder& instance();
		/**
		 * Parses the \a text in the following language:
		 *
		 * char			:= a-z
		 * word			:= char [char|SPACE]+
		 * query			:= "word"
		 * queryOrList := query | list
		 * op				:= | - U
		 * operator		:= $queryOrList [op queryOrList]+$
		 * queryOrOp	:= query | operator
		 * list			:= {queryOrOp [, queryOrOp]+}
		 */
		Query* buildQueryFrom(const QString& text, Model::Node* target);

		using QueryConstructor = std::function<Query* (Model::Node*, QStringList)>;

		void registerQueryConstructor(const QString& command, QueryConstructor constructor);

	private:
		QueryBuilder();
		enum class Type: int {Operator = 0, Query = 1, List = 2};
		Type typeOf(const QString& text);
		QPair<QStringList, QList<QChar> > split(const QString& text, const QList<QChar>& splitChars);

		Query* parseQuery(const QString& text);
		QList<Query*> parseList(const QString& text);
		Query* parseOperator(const QString& text, bool connectInput = false);
		QList<Query*> parseOperatorPart(const QString& text);

		QHash<QString, QueryConstructor> constructors_;
		Model::Node* target_{};
};

} /* namespace InformationScripting  */
