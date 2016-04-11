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

#include "Token.h"

#include "../OperatorDescriptor.h"
#include "../OperatorDescriptorList.h"

namespace Interaction {

QStringList Token::specialSignatureWords_ = {"SPACE", "EXPR", "ID"};

Token::Token()
{
}

Token::Token(QString text, Type type)
	: text_{text}, type_{type}
{}


QVector<Token> Token::tokenize(QString input, const OperatorDescriptorList* ops)
{
	QVector<Token> result;

	auto it = input.constBegin();
	while (it != input.constEnd())
	{
		auto oldIt = it;

		if (it->isLetter() || *it == '_') result.append(parseIdentifier(it, input.constEnd(), ops));
		else if (it->isDigit()) result.append(parseNumberLiteral(it, input.constEnd()));
		else if (*it == '"' || *it == '\'') result.append(parseStringLiteral(it, input.constEnd()));
		else result.append(parseOperator(it, input.constEnd(), ops));

		Q_ASSERT(!result.last().text().isEmpty());
		Q_ASSERT(it - oldIt == result.last().text().size());
	}

	// Remove all spaces, except for trailing ones
	// Only do this if quotes are matched. Otherwise we might end up in a situation where some spaces inside of
	// strings are accidentally removed while the expression is being modified.
	if (!result.empty() && result.last().type() != PartialLiteral)
	{
		bool trailing = true;
		QRegExp space{" +"};
		for (int i = result.length() - 1; i>=0;)
		{
			if (!space.exactMatch(result[i].text()))
			{
				trailing = false;
				--i;
				continue;
			}

			// We have a space
			if (trailing) --i;
			else result.removeAt(i);
		}
	}

	return result;
}

Token Token::parseStringLiteral(QString::const_iterator& it, const QString::const_iterator end)
{
	Q_ASSERT( it != end );
	Q_ASSERT( *it== '"' || *it == '\'');

	QString text = *it;
	QChar first = *it;
	bool escaped = false;

	++it;
	while (it != end)
	{
		text.append(*it);
		if (escaped) escaped = false;
		else
		{
			if (*it == '\\') escaped = true;
			else if (*it == first)
			{
				//finalize the string
				++it;
				return Token{text, Literal};
			}
		}

		++it;
	}

	// Return a partial (unfinished) token
	return Token{text, PartialLiteral};
}

Token Token::parseNumberLiteral(QString::const_iterator& it, const QString::const_iterator end)
{
	Q_ASSERT( it != end );
	Q_ASSERT( it->isDigit() );

	QString text = *it;
	++it;

	if (it == end) return Token{text, Literal};

	// Check for bin/hex
	if (text[0] == '0' && (*it == 'x' || *it == 'X' || *it == 'b' || *it == 'B') )
	{
		text.append(*it);
		++it;
	}

	auto accumulateDigitsAndSeparators = [&it, &end, &text](){
		while (it != end && (it->isDigit() || *it == '\'' || *it == '_'))
		{
			text.append(*it);
			++it;
		}
	};

	accumulateDigitsAndSeparators();

	if (it == end) return Token{text, Literal};

	if (*it == '.')
	{
		text.append(*it);
		++it;

		accumulateDigitsAndSeparators();
	}
	else if (*it == 'e' || *it == 'E')
	{
		text.append(*it);
		++it;

		if (it == end) return Token{text, Literal};

		if (*it == '+' || *it == '-')
		{
			text.append(*it);
			++it;
		}

		accumulateDigitsAndSeparators();
	}

	if (it == end) return Token{text, Literal};

	//Accumulate type specifiers
	while (it != end && (*it == 'l' || *it == 'L' || *it == 'f' || *it == 'F'
								|| *it == 'd' || *it == 'D' || *it == 'u' || *it == 'U'))
	{
		text.append(*it);
		++it;
	}

	return Token{text, Literal};
}

Token Token::parseIdentifier(QString::const_iterator& it, const QString::const_iterator end,
									  const OperatorDescriptorList* ops)
{
	Q_ASSERT( it != end );
	Q_ASSERT( it->isLetter() || *it == '_' );

	QString text = *it;
	++it;

	while (it != end && (it->isLetterOrNumber() || *it == '_'))
	{
		text.append(*it);
		++it;
	}

	// This might look like an identifier, but it could be a reserved keyword that represents an operator (e.g. 'delete')
	return Token{text, tokenExistsInOperators(text, ops) ? OperatorDelimiter : Identifier};
}

Token Token::parseOperator(QString::const_iterator& it, const QString::const_iterator end,
									const OperatorDescriptorList* ops)
{
	Q_ASSERT( it != end );
	Q_ASSERT( !it->isLetterOrNumber() && *it != '_'  && *it != '"' && *it != '\'');

	QString text = *it;
	++it;

	// See if adding more characters will yield an operator
	// Keep in mind that in this method we are only concerned with non-textual operators ( so no 'delete' for example).
	static constexpr int LOOK_AHEAD = 8; // Not quite sure anymore, why this number is 8
	auto finalLookAheadPos = it + LOOK_AHEAD;
	QString potentialOperator = text;

	for (auto peekIterator = it; peekIterator != end && peekIterator != finalLookAheadPos; ++peekIterator)
	{
		potentialOperator += *peekIterator;
		if ( tokenExistsInOperators(potentialOperator, ops) )
		{
			text = potentialOperator;
			it = peekIterator+1;
		}
	}

	return Token{text, OperatorDelimiter};
}

bool Token::tokenExistsInOperators(QString token, const OperatorDescriptorList* ops)
{
	for (int i = 0; i < ops->size(); ++i)
		if (ops->at(i)->signature().contains(token) )
			return OperatorDescriptor::isDelimiter(token) && !specialSignatureWords_.contains(token);

	return false;
}

QVector<Token> Token::createSubExpressions(const QVector<Token>& tokens)
{
	QVector<QChar> parens = {'(', ')', '{', '}', '[', ']'};

	QMultiMap<int, QPair<QChar, QChar>> parensSorted;
	for (int i = 0; i<parens.size(); i+=2)
		parensSorted.insert(
				countUnmatched(tokens.begin(), tokens.end(), parens[i], parens[i+1]), qMakePair(parens[i], parens[i+1]));

	QVector<Token> result = tokens;
	for (auto parens : parensSorted.values())
		result = createSubExpressions(result.begin(), result.end(), parens.first, parens.second);

	return result;
}

QVector<Token> Token::createSubExpressions(QVector<Token>::const_iterator start, QVector<Token>::const_iterator end,
		QChar openParen, QChar closeParen)
{
	auto splitStart = start;
	auto splitEnd = end;

	bool foundSplit = findSplit(splitStart, splitEnd, openParen, closeParen);

	if (!foundSplit) splitStart = end;
	QVector<Token> result;

	//Add all initial tokens
	auto it = start;
	while (it != splitStart)
	{
		result.append(*it);
		if ( result.last().type_ == SubExpression )
			// Look into already created sub expressions from other types of parenthesis
			result.last().subExpressionTokens_ = createSubExpressions(result.last().subExpressionTokens_.begin(),
					result.last().subExpressionTokens_.end(), openParen, closeParen);
		++it;
	}

	if (foundSplit)
	{
		// Create sub expression
		result.append(Token{QString{}, SubExpression});
		Token& sub = result.last();
		sub.subExpressionTokens_ = createSubExpressions(splitStart, splitEnd, openParen, closeParen);
		for (auto t : sub.subExpressionTokens_) sub.text_ += t.text_;

		// Add trailing tokens
		for (auto t : createSubExpressions(splitEnd, end, openParen, closeParen))
		result.append(t);
	}

	return result;
}

int Token::countUnmatched(QVector<Token>::const_iterator start, QVector<Token>::const_iterator end,
		QChar openParen, QChar closeParen)
{
	int current = 0;
	int unmatched = 0;
	while (start != end)
	{
		if (start->type_ == OperatorDelimiter)
		{
			if (start->text_ == openParen) ++current;
			else if (start->text_ == closeParen) --current;
		}

		if (current < 0)
		{
			++unmatched;
			current = 0;
		}

		++start;
	}

	return unmatched + current;
}

/**
 * Adjusts the \a splitStart and \a splitEnd arguments to indicate the best split of the given range and returns true
 * if a split was found.
 *
 * If the return value is false the input sequence of tokens should remain as it is.
 *
 * The range is given by the initial values of \a splitStart and \a splitEnd. The returned values are such that:
 * tokens before \a splitStart should remain as they are (there is no open paren); \a splitStart points just after the
 * first open paren; \a splitEnd points at the closing paren that matches the one from \a splitStart.
 *
 * The best possible split is determined by trying to minimize the sum of the unmatched count of the tokens:
 * between \a splitStart and \a splitEnd; after \a splitEnd
 */
bool Token::findSplit(QVector<Token>::const_iterator& splitStart, QVector<Token>::const_iterator& splitEnd,
				QChar openParen, QChar closeParen)
{
	// Find open paren
	while (splitStart != splitEnd && (splitStart->type_ != OperatorDelimiter || splitStart->text_ != openParen))
		++splitStart;

	if (splitStart == splitEnd) return false;
	++splitStart; // Advance to just after the first open paren
	if (splitStart == splitEnd) return false;

	auto splitMid = splitStart;
	TokenSplitData bestSplit;

	int leftBalance = 0;
	int leftUnmatched = 0;
	bool foundSplit = false;
	do
	{
		if (splitMid->type_ == OperatorDelimiter)
		{
			if (splitMid->text_ == openParen)
			{
				++leftBalance;
			}
			else if (splitMid->text_ == closeParen)
			{
				//Try using this as a closing paren for splitStart


				--leftBalance;
				if (leftBalance < 0)
				{
					leftBalance = 0;
					++leftUnmatched;
				}

				TokenSplitData candidateSplit{splitMid, leftBalance + leftUnmatched,
					countUnmatched(splitMid, splitEnd, openParen, closeParen)};

				if (!foundSplit || candidateSplit < bestSplit) bestSplit = candidateSplit;

				foundSplit = true;
			}
		}

		++splitMid;
	} while ( splitMid != splitEnd);

	if (foundSplit) splitEnd = bestSplit.end;
	return foundSplit;
}

bool operator< (const TokenSplitData& left, const TokenSplitData& right)
{
	auto leftTotal = left.unmatchedBeforeEnd + left.unmatchedAfterEnd;
	auto rightTotal = right.unmatchedBeforeEnd + right.unmatchedAfterEnd;

	if (leftTotal < rightTotal) return true;
	if (leftTotal > rightTotal) return false;

	// Here it's possible to explore alternatives that will result in a different split
	// For example: return left.unmatchedBeforeEnd < right.unmatchedAfterEnd;
	return left.unmatchedAfterEnd < right.unmatchedAfterEnd;
}

}
