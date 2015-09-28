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

class CommandNode;
class CompositeQueryNode;
class OperatorNode;
class QueryNode;

class INFORMATIONSCRIPTING_API QueryParser
{
	public:
		static QueryParser& instance();

		QueryNode* parse(const QString& text);

	private:
		QueryParser() = default;
		enum class Type: int {Operator = 0, Command = 1, List = 2};
		Type typeOf(const QString& text);
		QPair<QStringList, QList<QChar> > split(const QString& text, const QList<QChar>& splitChars);

		CommandNode* parseCommand(const QString& text);
		CompositeQueryNode* parseList(const QString& text);
		OperatorNode* parseOperator(const QString& text);
		QueryNode* parseOperatorPart(const QString& text);

		static constexpr int SCOPE_SYMBOL_LENGTH_{2};
		static const QStringList OPEN_SCOPE_SYMBOL;
		static const QStringList CLOSE_SCOPE_SYMBOL;
};

} /* namespace InformationScripting  */
