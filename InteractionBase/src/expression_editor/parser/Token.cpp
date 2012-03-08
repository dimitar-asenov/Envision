/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * Token.cpp
 *
 *  Created on: Jan 11, 2012
 *      Author: Dimitar Asenov
 */

#include "expression_editor/parser/Token.h"

#include "expression_editor/OperatorDescriptor.h"
#include "expression_editor/OperatorDescriptorList.h"

namespace Interaction {

Token::Token()
{
}

Token::Token(QString text, Type type)
	: text_(text), type_(type)
{}


QVector<Token> Token::tokenize(QString input, const OperatorDescriptorList* ops)
{
	QVector<Token> result;
	input = input.trimmed();
	if (input.isEmpty()) return result;
	input.append(' ');

	QChar first;
	QString token;
	bool escaped = false;
	bool inString = false;
	int openingStringQuoteIndex = -1;

	for(int i = 0; i<input.size(); ++i )
	{
		if (token.isEmpty() && input[i] == ' ') continue;

		// If we have an unfinished string
		if (i == input.size() -1 && inString && (escaped || input[i] != '"'))
		{
			// If the string was just opened, close it automatically
			if (openingStringQuoteIndex == i-1)
			{
				input.insert(i,'"');
			}
			else
			{
				// Do not treat it as string.
				// Roll back and interpret the rest as normal characters
				result.append(Token("\"", OperatorDelimiter));
				token = "";
				i = openingStringQuoteIndex;
				escaped = false;
				inString = false;
				continue;
			}
		}

		QChar ch = input[i];

		if (token.isEmpty())
		{
			token = ch;
			first = ch;
			if (ch == '"')
			{
				inString = true;
				openingStringQuoteIndex = i;
			}
			continue;
		}

		// Finalize token
		if ((!inString
				&& (ch == ' '
					|| first.isLetterOrNumber() != ch.isLetterOrNumber()
					|| (!ch.isLetterOrNumber() && !tokenExistsInOperators(token + ch, ops))))
			|| (inString && ch == '"' && !escaped))
		{
			Type t;
			if (inString)
			{
				token.append(ch);
				t = Literal;
			}
			else if (tokenExistsInOperators(token, ops) || (token.size() == 1 && !first.isLetterOrNumber()))
				t = OperatorDelimiter;
			else
				t = first.isDigit() ? Literal : Identifier;

			result.append(Token(token,t));
			token = "";


			if (inString)
			{
				inString = false;
				escaped = false;
				continue;
			}
			else
			{
				first = ch;
				if (ch == '"')
				{
					inString = true;
					openingStringQuoteIndex = i;
				}
			}
		}

		// Add current symbol to token
		if (inString || ch != ' ') token += ch;

		if (inString && ch == '\\' && !escaped) escaped = true;
		else if (inString && escaped) escaped = false;
	}

	return result;
}

bool Token::tokenExistsInOperators(QString token, const OperatorDescriptorList* ops)
{
	for (int i = 0; i < ops->size(); ++i)
		if (ops->at(i)->signature().contains(token) ) return true;

	return false;
}

} /* namespace InteractionBase */
