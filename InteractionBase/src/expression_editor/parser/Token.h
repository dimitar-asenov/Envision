/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#pragma once

#include "../../interactionbase_api.h"
#include "ParseResult.h"

namespace Interaction {

class OperatorDescriptorList;
class ExpressionTreeBuildInstruction;
class Parser;

class INTERACTIONBASE_API Token {
	public:
		enum Type {
			Identifier, /**< This token is an identifier. It is not identical to a keyword */
			Literal, /**< This token is a string or a number literal. */
			OperatorDelimiter,	/**< This token is an operator delimiter - a keyword or an operator,
										including parenthesis */
			PartialLiteral, /**< This token is a partial literal, such an incomplete string. */
			SubExpression 	/**< This token is a sub expression. This is used with expressions surrounded by braces or
			 	 	 	 		parenthesis to speed up the parsing process. */};

		Token();
		Token(QString text, Type type);

		const QString& text() const;
		Type type() const;

		static QVector<Token> tokenize(QString input, const OperatorDescriptorList* ops);
		static QVector<Token> createSubExpressions(const QVector<Token>& tokens);

	private:
		QString text_;
		Type type_;

		//The members below are only used with SubExpression type literals
		friend class Parser;
		QVector<Token> subExpressionTokens_{};
		ParseResult subExpressionResult_{};

		static bool tokenExistsInOperators(QString token, const OperatorDescriptorList* ops);
		static QStringList specialSignatureWords_;

		//These are used when creating sub expressions
		static int countUnmatched(QVector<Token>::const_iterator start, QVector<Token>::const_iterator end,
				QChar openParen, QChar closeParen);

		static bool findSplit(QVector<Token>::const_iterator& splitStart, QVector<Token>::const_iterator& splitEnd,
				QChar openParen, QChar closeParen);

		static QVector<Token> createSubExpressions(QVector<Token>::const_iterator start,
				QVector<Token>::const_iterator end, QChar openParen, QChar closeParen);
};

inline const QString& Token::text() const { return text_; }
inline Token::Type Token::type() const { return type_; }

struct INTERACTIONBASE_API TokenSplitData {
	QVector<Token>::const_iterator end;
	int unmatchedBeforeEnd;
	int unmatchedAfterEnd;
};
bool operator< (const TokenSplitData& left, const TokenSplitData& right);

}
