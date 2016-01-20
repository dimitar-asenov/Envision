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

#include "Parser.h"

#include "../tree_builder/AddEmptyValue.h"
#include "../tree_builder/AddErrorOperator.h"
#include "../tree_builder/AddOperator.h"
#include "../tree_builder/AddValue.h"
#include "../tree_builder/FinishOperator.h"
#include "../tree_builder/LeaveUnfinished.h"
#include "../tree_builder/SkipOperatorDelimiter.h"
#include "../tree_builder/AddSubExpression.h"

#include "../OperatorDescriptorList.h"
#include "../OperatorDescriptor.h"

#include "../../InteractionBaseException.h"

namespace Interaction {

Parser::Parser(const OperatorDescriptorList* ops) : ops_{ops}
{
}

QVector<ExpressionTreeBuildInstruction*> Parser::parse(QVector<Token> tokens)
{
	ParseResult best;
	return parse(tokens, best);
}

QVector<ExpressionTreeBuildInstruction*> Parser::parse(QVector<Token> tokens, ParseResult& parseResult)
{
	endTokens_ = tokens.end();

	QList<ExpectedToken> expected;
	expected << ExpectedToken();

	QVector<ExpressionTreeBuildInstruction*> instructions = QVector<ExpressionTreeBuildInstruction*>();

	if (tokens.isEmpty())
	{
		instructions.append( new AddEmptyValue{} );
		return instructions;
	}
	else
	{
		ParseResult worst;
		worst.errors = INT_MAX;
		parseResult = parse(tokens.begin(), ParseResult(), expected, false, instructions, worst);
		return parseResult.instructions;
	}
}

ParseResult Parser::parse(QVector<Token>::iterator token, ParseResult result, QList<ExpectedToken>& expected,
		bool hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions, ParseResult& bestParseSoFar)
{
	// Finish any completed operators
	while (!expected.isEmpty() && expected.first().type == ExpectedToken::END)
	{
		expected.removeFirst();
		instructions.append(new FinishOperator{});
		result.numOperators++;
		hasLeft = true;
	}

	// Get a lower bound on trailing tokens.
	// result.missingInnerTokens is only allowed to grow AND
	// result.missingInnerTokens + result.missingTrailingTokens should also only grow.
	// Together these two conditions are necessary to assure that the prunning to work correctly.
	// The rest of this invariant is maintained in processExpectedOperatorDelimiters.
	int numTokens = endTokens_ - token;
	for (auto e : expected)
		if (e.type == ExpectedToken::FIRST_DELIM || e.type == ExpectedToken::FOLLOWING_DELIM)
			--numTokens;
	// Only count delimiters, since values/ids/types could be recorded in result.emptyExpressions

	if (numTokens < 0)
	{
		Q_ASSERT(result.missingTrailingTokens <= -numTokens);
		result.missingTrailingTokens = -numTokens;
	}
	else result.missingTrailingTokens = 0;

	// Prune the search
	if (bestParseSoFar < result) return result;

	// Finish parsing if the end is reached
	if (token == endTokens_)
	{
		Q_ASSERT( result.missingTrailingTokens <= expected.size() );
		result.missingTrailingTokens = expected.size();
		result.instructions = instructions;
		if (! (bestParseSoFar < result)) bestParseSoFar = result;
		return result;
	}

	bool error = false;

	bool identifierOrLiteral = token->type() == Token::Identifier || token->type() == Token::Literal;
	bool unexpectedIdentifierOrLiteral = identifierOrLiteral &&
			(expected.isEmpty() || expected.first().type == ExpectedToken::FIRST_DELIM
			 || expected.first().type == ExpectedToken::FOLLOWING_DELIM);

	if (identifierOrLiteral && !unexpectedIdentifierOrLiteral )
		processIdentifiersAndLiterals(error, expected, token, hasLeft, instructions);
	else if (token->type() == Token::PartialLiteral)
		error = true;
	else if (token->type() == Token::SubExpression)
		processSubExpression(error, expected, token, hasLeft, result, instructions);
	else if (token->type() == Token::OperatorDelimiter || unexpectedIdentifierOrLiteral)
	{
		// Try two things: interpret this delimiter as part of an existing operator or as introducing a new one
		// Return whichever produces a better result.

		// If the current token is the same as any of the next expected delimiters try to complete the parsing assuming
		// missing expressions in between and finishing the intermediate operators.
		bool processedByExpectedDelimiters = false;

		ParseResult pr_expected_delim;
		if (!unexpectedIdentifierOrLiteral)
			pr_expected_delim = processExpectedOperatorDelimiters(processedByExpectedDelimiters, expected, token,
					result, instructions, bestParseSoFar);

		// Try to complete the parsing assuming that a new operator is being introduced by the current token.
		bool processedByNewDelimiters = false;
		processNewOperatorDelimiters(processedByNewDelimiters, error, expected, token, hasLeft, result, instructions,
											  bestParseSoFar, unexpectedIdentifierOrLiteral);

		// Return the better result if any.
		if (processedByExpectedDelimiters && !processedByNewDelimiters) return pr_expected_delim;
		if (!processedByExpectedDelimiters && processedByNewDelimiters) return result;
		if (processedByExpectedDelimiters && processedByNewDelimiters)
			return result < pr_expected_delim ? result : pr_expected_delim;

		error = true;
	}
	else Q_ASSERT(false); // Unknown token type

	// We do not get here if this is an OperatorDelimiter which has been processed.
	// That case returns early.
	if (error)
	{
		if (token->type() == Token::SubExpression)
			result.errors += token->subExpressionTokens_.size();
		else
			++result.errors;
		instructions.append( new AddErrorOperator{token->text()} );

		if (!hasLeft) expected.insert(1, ExpectedToken(ExpectedToken::END));
	}

	++token;
	return parse(token, result, expected, hasLeft, instructions, bestParseSoFar);
}

void Parser::processIdentifiersAndLiterals(bool& error, QList<ExpectedToken>& expected,
		QVector<Token>::iterator& token, bool& hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	if ( !expected.isEmpty() &&
			(	expected.first().type == ExpectedToken::VALUE
			|| expected.first().type == ExpectedToken::ANY
			|| expected.first().type == ExpectedToken::TYPE // TYPE should not be limited to Identifiers, because there
			// might be a postfix operator that converts a literal to a type (e.g. 123 <typeof)
			|| (expected.first().type == ExpectedToken::ID && token->type() == Token::Identifier)))
	{
		instructions.append( new AddValue{token->text()} );
		hasLeft = true;
		expected.removeFirst();
		error = false;
	}
	else error = true;
}

ParseResult Parser::processExpectedOperatorDelimiters(bool& processed, QList<ExpectedToken>& expected,
		QVector<Token>::iterator& token, ParseResult& result,
		QVector<ExpressionTreeBuildInstruction*>& instructions, ParseResult& bestParseSoFar)
{
	// We should only fill using empty expressions the first unfinished operator. Any operator afterwards should be left
	// untouched.
	bool fillMissingWithEmptyExpressions = true;

	// We can only use the current token to complete the first unseen delimiter of unfinished operators.
	// In case unfinished operators have more than one unseen delimiter, only the first one will be tried and the rest
	// skipped. The reason for this is mainly that we have no way of finishing an operator "in parts" e.g. a bit from
	// the beginning and a bit from the end.
	bool firstUnseenDelimiter = true;

	ParseResult best_pr = result;
	for (int index = 0; index<expected.size(); ++index)
	{
		bool expectedIsEnd = expected.at(index).type == ExpectedToken::END;
		bool expectedIsDelimiter = false;

		bool tokensMatch = expected.at(index).text == token->text();

		if (expected.at(index).type == ExpectedToken::FIRST_DELIM)
		{
			expectedIsDelimiter = true;

			int nextIndex = index+1;
			QVector<Token>::iterator nextToken = token + 1;
			while (tokensMatch && nextIndex < expected.size()
					&& expected.at(nextIndex).type == ExpectedToken::FOLLOWING_DELIM )
			{
				//Check if the following delimiters match
				if (nextToken != endTokens_ && nextToken->type() == Token::OperatorDelimiter &&
					 expected.at(nextIndex).text == nextToken->text())
				{
					++nextToken;
					++nextIndex;
				}
				else tokensMatch = false;
			}
		}
		else if (expected.at(index).type == ExpectedToken::FOLLOWING_DELIM)
		{
			expectedIsDelimiter = true;
			//For this to be OK the previous instruction must have been SkipOperatorDelimiter
			Q_ASSERT(!instructions.isEmpty());
			if (dynamic_cast<AddOperator*>(instructions.last()) == nullptr &&
				 dynamic_cast<SkipOperatorDelimiter*>(instructions.last()) == nullptr)
				tokensMatch = false;
		}

		if ( expectedIsDelimiter && firstUnseenDelimiter && tokensMatch)
		{
			auto new_expected = expected;
			ParseResult pr = result;
			QVector<ExpressionTreeBuildInstruction*> new_instructions = instructions;

			// Finish all intermediate positions
			for (int i = 0; i<index; ++i)
			{
				auto exp = new_expected.takeFirst();

				if (exp.type == ExpectedToken::VALUE || exp.type == ExpectedToken::ID
						|| exp.type == ExpectedToken::TYPE  || exp.type == ExpectedToken::ANY )
				{
					if (fillMissingWithEmptyExpressions) new_instructions.append( new AddEmptyValue{} );
					++pr.emptyExpressions;
				}
				else if (exp.type == ExpectedToken::END)
				{
					// If the expectation is not one of the above then it must be either an end or a delimiter
					new_instructions.append(fillMissingWithEmptyExpressions ?
							(ExpressionTreeBuildInstruction*)new FinishOperator{} : new LeaveUnfinished{});
					++pr.numOperators;
				}
				else // The expectation is a delimiter.
				{
					// A missing inner token could have already been assumed to be a trailing token.
					// As soon as a new missing inner token is discovered, the corresponding trailing token count should be
					// decreased
					if (pr.missingTrailingTokens >= 1) --pr.missingTrailingTokens;
					++pr.missingInnerTokens;
				}
			}

			// Finish the actual delimiter
			new_expected.removeFirst();
			new_instructions.append(new SkipOperatorDelimiter{});

			// Assume the finished delimiter was an infix one, therefore there is no 'left' expression
			// If the finished delimiter is a postfix the last unfinished operator will be finished
			// in the beginning of the next call to parse() and then 'hasLeft' will be correctly set
			pr = parse(token + 1, pr, new_expected, false, new_instructions, bestParseSoFar);
			if (!processed || pr < best_pr)
			{
				processed = true;
				best_pr = pr;
			}
		}

		if (expectedIsEnd) firstUnseenDelimiter = true;
		else if (expectedIsDelimiter)
		{
			// The first time we encounter a delimiter we no longer should fill missing places with empty expressions
			fillMissingWithEmptyExpressions = false;
			firstUnseenDelimiter = false;
		}
	}

	return best_pr;
}

void Parser::processNewOperatorDelimiters(bool& processed, bool& error, QList<ExpectedToken>& expected,
		QVector<Token>::iterator& token, bool& hasLeft, ParseResult& result,
		QVector<ExpressionTreeBuildInstruction*>& instructions, ParseResult& bestParseSoFar,
		bool unexpectedIdentifierOrLiteral)
{
	if (!unexpectedIdentifierOrLiteral && !expected.isEmpty() &&
		 ( expected.first().type == ExpectedToken::ID || expected.first().type == ExpectedToken::FOLLOWING_DELIM))
	{
		error = true;
	}
	else
	{
		bool prefix = !unexpectedIdentifierOrLiteral && !expected.isEmpty() &&
				( 	expected.first().type == ExpectedToken::VALUE
					|| expected.first().type == ExpectedToken::TYPE
					|| expected.first().type == ExpectedToken::ANY);

		QList<OperatorDescriptor*> matching_ops;
		if (unexpectedIdentifierOrLiteral)
		{
			matching_ops.append( ops_->findByEmptyInfixWithoutPrefix());
		}
		else
		{
			QStringList allConsecutiveOperatorTokens = {token->text()};
			QVector<Token>::iterator nextToken = token + 1;
			while (nextToken != endTokens_ && nextToken->type() == Token::OperatorDelimiter)
			{
				allConsecutiveOperatorTokens.append(nextToken->text());
				++nextToken;
			}


			if (prefix) {
				matching_ops.append( ops_->findByPrefix(allConsecutiveOperatorTokens) );
			}
			else {
				if (hasLeft)
				{
					matching_ops.append( ops_->findByInfixWithoutPrefix(allConsecutiveOperatorTokens) );
					matching_ops.append( ops_->findByPostfixWithoutPreInfix(allConsecutiveOperatorTokens) );
				}
				else
				{
					// This situation arises for example in the 'delete []' operator where two different operator tokens
					// follow one another. It is handled by processExpectedOperatorDelimiters()
				}

			}
		}

		if (matching_ops.isEmpty())
		{
			error = true;
			return;
		}

		processed = true;
		if (prefix) expected.removeFirst();

		ParseResult best_pr;
		OperatorDescriptor* best_oi = nullptr;

		for (OperatorDescriptor* oi : matching_ops)
		{
			auto new_expected = oi->expectedTokens();
			new_expected.removeFirst(); // Remove the prefix/expr token from the signature
			if (!prefix && !unexpectedIdentifierOrLiteral)
				new_expected.removeFirst(); // Remove the in/postfix token from the signature
			new_expected.append( expected );


			QVector<ExpressionTreeBuildInstruction*> new_instructions = instructions;
			new_instructions.append( new AddOperator{oi} );
			ParseResult pr = parse(token + (unexpectedIdentifierOrLiteral ? 0 : 1), result, new_expected,
										  false, new_instructions, bestParseSoFar);

			if (best_oi == nullptr || pr < best_pr)
			{
				best_pr = pr;
				best_oi = oi;
			}
		}

		// Simply return the best result
		result = best_pr;
	}
}

void Parser::processSubExpression(bool& error, QList<ExpectedToken>& expected, QVector<Token>::iterator& token,
	bool& hasLeft, ParseResult& result, QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	if ( !hasLeft && !expected.isEmpty() &&
		  (	expected.first().type == ExpectedToken::VALUE
				|| expected.first().type == ExpectedToken::ANY
				|| expected.first().type == ExpectedToken::TYPE)
		  )
	{
		if (token->subExpressionResult_.instructions.isEmpty())
		{
			// If this is the first time this token was encountered, parse the sub expression
			if (token->subExpressionTokens_.isEmpty())
				token->subExpressionResult_.instructions.append( new AddEmptyValue{} );
			else
			{
				//A new parser must be created since we need a new endTokens_ field.
				Parser(*this).parse(token->subExpressionTokens_, token->subExpressionResult_);
			}
		}

		// The sub expression is already parsed, use its result

		instructions.append( new AddSubExpression{token->subExpressionResult_.instructions});
		result.errors += token->subExpressionResult_.errors;
		result.emptyExpressions += token->subExpressionResult_.emptyExpressions;
		result.missingInnerTokens += token->subExpressionResult_.missingInnerTokens;
		result.numOperators += token->subExpressionResult_.numOperators;
		// Note that we ignore missingTrailingTokens on purpose.

		hasLeft = true;
		expected.removeFirst();
	}
	else error = true;
}

}
