/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "expression_editor/parser/Parser.h"

#include "expression_editor/tree_builder/AddEmptyValue.h"
#include "expression_editor/tree_builder/AddErrorOperator.h"
#include "expression_editor/tree_builder/AddOperator.h"
#include "expression_editor/tree_builder/AddValue.h"
#include "expression_editor/tree_builder/FinishOperator.h"
#include "expression_editor/tree_builder/LeaveUnfinished.h"
#include "expression_editor/tree_builder/SkipOperatorDelimiter.h"
#include "expression_editor/tree_builder/AddSubExpression.h"

#include "expression_editor/OperatorDescriptorList.h"
#include "expression_editor/OperatorDescriptor.h"

#include "../../InteractionBaseException.h"

namespace Interaction {

Parser::Parser(const OperatorDescriptorList* ops) : ops_(ops)
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
		instructions.append( new AddEmptyValue() );
		return instructions;
	}
	else
	{
		parseResult = parse(tokens.begin(), ParseResult(), expected, false, instructions);
		return parseResult.instructions;
	}
}

ParseResult Parser::parse(QVector<Token>::iterator token, ParseResult result, QList<ExpectedToken>& expected,
		bool hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	// Finish any completed operators
	while (!expected.isEmpty() && expected.first().type == ExpectedToken::END)
	{
		expected.removeFirst();
		instructions.append(new FinishOperator());
		result.numOperators++;
		hasLeft = true;
	}

	// Finish parsing if the end is reached
	if (token == endTokens_)
	{
		result.missingTrailingTokens = expected.size();
		result.instructions = instructions;
		return result;
	}

	bool error = false;
	switch(token->type())
	{
		case Token::Identifier:
		case Token::Literal:
		{
			processIdentifiersAndLiterals(error, expected, token, hasLeft, instructions);
			break;
		}
		case Token::OperatorDelimiter:
		{
			// Try two things: interpret this delimiter as part of an existing operator or as introducing a new one
			// Return whichever produces a better result.

			// If the current token is the same as any of the next expected delimiters try to complete the parsing assuming
			// missing expressions in between and finishing the intermediate operators.
			bool processedByExpectedDelimiters = false;
			auto pr_expected_delim = processExpectedOperatorDelimiters(processedByExpectedDelimiters, expected, token,
						result, instructions);

			// Try to complete the parsing assuming that a new operator is being introduced by the current token.
			bool processedByNewDelimiters = false;
			processNewOperatorDelimiters(processedByNewDelimiters, error, expected, token, hasLeft, result, instructions);

			// Return the better result if any.
			if (processedByExpectedDelimiters && !processedByNewDelimiters) return pr_expected_delim;
			if (!processedByExpectedDelimiters && processedByNewDelimiters) return result;
			if (processedByExpectedDelimiters && processedByNewDelimiters)
				return result < pr_expected_delim ? result : pr_expected_delim;

			error = true;
			break;
		}
		case Token::PartialLiteral:
		{
			error = true;
			break;
		}
		case Token::SubExpression:
		{
			processSubExpression(error, expected, token, hasLeft, result, instructions);
			break;
		}
		default:
			Q_ASSERT(false); // Unknown token type
	}

	// We do not get here if this is an OperatorDelimiter which has been processed.
	// That case returns early.
	if (error)
	{
		if (token->type() == Token::SubExpression)
			result.errors += token->subExpressionTokens_.size();
		else
			++result.errors;
		instructions.append( new AddErrorOperator(token->text()) );
		if (!hasLeft) expected.insert(1, ExpectedToken(ExpectedToken::END));
	}

	++token;
	return parse(token, result, expected, hasLeft, instructions);
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
		instructions.append( new AddValue(token->text()) );
		hasLeft = true;
		expected.removeFirst();
		error = false;
	}
	else error = true;
}

ParseResult Parser::processExpectedOperatorDelimiters(bool& processed, QList<ExpectedToken>& expected,
		QVector<Token>::iterator& token, ParseResult& result,
		QVector<ExpressionTreeBuildInstruction*>& instructions)
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
		bool expectedIsDelimiter = expected.at(index).type == ExpectedToken::DELIM;

		if ( expectedIsDelimiter && firstUnseenDelimiter && expected.at(index).text == token->text())
		{
			auto new_expected = expected;
			ParseResult pr = result;
			QVector<ExpressionTreeBuildInstruction*> new_instructions = instructions;

			pr.missingInnerTokens += index;
			// Finish all intermediate positions
			for (int i = 0; i<index; ++i)
			{
				auto exp = new_expected.takeFirst();

				if (exp.type == ExpectedToken::VALUE || exp.type == ExpectedToken::ID
						|| exp.type == ExpectedToken::TYPE  || exp.type == ExpectedToken::ANY )
				{
					if (fillMissingWithEmptyExpressions) new_instructions.append( new AddEmptyValue() );
					++pr.emptyExpressions;
				}
				else if (exp.type == ExpectedToken::END)
				{
					// If the expectation is not one of the above then it must be either an end or a delimiter
					new_instructions.append(fillMissingWithEmptyExpressions ?
							(ExpressionTreeBuildInstruction*)new FinishOperator() : new LeaveUnfinished());
					pr.numOperators++;
				}
				// Do nothing in case we are skipping over a delimiter.
			}

			// Finish the actual delimiter
			new_expected.removeFirst();
			new_instructions.append(new SkipOperatorDelimiter());

			// Assume the finished delimiter was an infix one, therefore there is no 'left' expression
			// If the finished delimiter is a postfix the last unfinished operator will be finished
			// in the beginning of the next call to parse() and then 'hasLeft' will be correctly set
			pr = parse(token + 1, pr, new_expected, false, new_instructions);
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
		QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	if (!expected.isEmpty() && expected.first().type == ExpectedToken::ID)
	{
		error = true;
	}
	else
	{
		bool prefix = !expected.isEmpty() &&
				( 	expected.first().type == ExpectedToken::VALUE
					|| expected.first().type == ExpectedToken::TYPE
					|| expected.first().type == ExpectedToken::ANY);

		QList<OperatorDescriptor*> matching_ops;
		if (prefix) {
			matching_ops.append( ops_->findByPrefix(token->text()) );
		}
		else {
			if (hasLeft)
			{
				matching_ops.append( ops_->findByInfixWithoutPrefix(token->text()) );
				matching_ops.append( ops_->findByPostfixWithoutPreInfix(token->text()) );
			}
			else
			{
				// This situation arises for example in the 'delete []' operator where two different operator tokens follow
				// one another. It is handled by processExpectedOperatorDelimiters()
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
			if (!prefix) new_expected.removeFirst(); // Remove the in/postfix token from the signature
			new_expected.append( expected );


			QVector<ExpressionTreeBuildInstruction*> new_instructions = instructions;
			new_instructions.append( new AddOperator(oi) );
			ParseResult pr = parse(token + 1, result, new_expected, false, new_instructions);

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
				token->subExpressionResult_.instructions.append( new AddEmptyValue() );
			else
			{
				//A new parser must be created since we need a new endTokens_ field.
				Parser(ops_).parse(token->subExpressionTokens_, token->subExpressionResult_);
			}
		}

		// The sub expression is already parsed, use its result

		instructions.append( new AddSubExpression(token->subExpressionResult_.instructions));
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

} /* namespace InteractionBase */
