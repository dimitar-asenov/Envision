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
 * Parser.cpp
 *
 *  Created on: Jan 11, 2012
 *      Author: Dimitar Asenov
 */

#include "expression_editor/parser/Parser.h"

#include "expression_editor/tree_builder/AddEmptyValue.h"
#include "expression_editor/tree_builder/AddErrorOperator.h"
#include "expression_editor/tree_builder/AddOperator.h"
#include "expression_editor/tree_builder/AddValue.h"
#include "expression_editor/tree_builder/FinishOperator.h"
#include "expression_editor/tree_builder/SkipOperatorDelimiter.h"

#include "expression_editor/OperatorDescriptorList.h"
#include "expression_editor/OperatorDescriptor.h"

namespace Interaction {

Parser::Parser(const OperatorDescriptorList* ops) : ops_(ops)
{
}

QVector<ExpressionTreeBuildInstruction*> Parser::parse(QVector<Token> tokens)
{

	end_tokens_ = tokens.constEnd();

	QStringList expected;
	expected.append("expr");

	QVector<ExpressionTreeBuildInstruction*> instructions = QVector<ExpressionTreeBuildInstruction*>();

	if (tokens.isEmpty())
	{
		instructions.append( new AddEmptyValue());
		return instructions;
	}
	else
	{
		// First run a simulation to obtain the best result
		ParseResult best = parse(tokens.constBegin(), ParseResult(), expected, false, instructions);
		return best.instructions;
	}
}

ParseResult Parser::parse(QVector<Token>::const_iterator token, ParseResult result, QStringList& expected, bool hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	// Finish any completed operators
	while (!expected.isEmpty() && expected.first() == "end")
	{
		expected.removeFirst();
		instructions.append(new FinishOperator());
		hasLeft = true;
	}

	if (token == end_tokens_)
	{
		result.missing_trailing_tokens = expected.size();
		result.instructions = instructions;
		return result;
	}

	bool processed = false;
	bool error = false;

	// If the current token is the same as the next expected delimiter
	// put empty expressions where required and finish the intermediate operators
	processNextExpectedDelimiter(processed, expected, token, hasLeft, result, instructions);

	if (!processed && (token->type() == Token::Identifier || token->type() == Token::Literal) )
		processIdentifiersAndLiterals(processed, error, expected, token, hasLeft, instructions);

	if (!processed && token->type() == Token::OperatorDelimiter)
	{
		processOperatorDelimiters(processed, error, expected, token, hasLeft, result, instructions);
		if (processed) return result; // No need to redo the rest of the work as it has been done here.
	}

	if (!processed || error)
	{
		++result.errors;
		instructions.append( new AddErrorOperator(token->text()) );
		if (!hasLeft) expected.insert(1, "end");
	}

	++token;
	return parse(token, result, expected, hasLeft, instructions);
}

QString Parser::getNextExpectedDelimiter(const QStringList& expected, int& index)
{
	for (index = 0; index<expected.size(); ++index)
		if (expected.at(index) != "expr" && expected.at(index) != "id" && expected.at(index) != "end")
			return expected.at(index);

	return QString();
}

void Parser::processNextExpectedDelimiter(bool& processed, QStringList& expected, QVector<Token>::const_iterator& token, bool& hasLeft, ParseResult& result, QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	int index;
	QString next_delim = getNextExpectedDelimiter(expected, index);

	if (next_delim.isEmpty() || next_delim != token->text()) return;

	processed = true;
	result.missing_inner_tokens += index;

	// Finish all intermediate positions
	for (int i = 0; i<index; ++i)
	{
		if (expected.first() == "expr" || expected.first() == "id" )
		{
			expected.removeFirst();
			instructions.append( new AddEmptyValue() );
		}
		else
		{
			// If the expectation is not an expression or an identifier then it must be an end
			expected.removeFirst();
			instructions.append(new FinishOperator());
		}
	}

	// Finish the actual delimiter
	expected.removeFirst();
	instructions.append(new SkipOperatorDelimiter());

	// Assume the finished delimtier was an infix one, therefore no left expression
	// If the finished delimiter is a postfix the last unfinished operator will be finished
	// in the beginning of the next call to parse() and then 'hasLeft' will be correctly set
	hasLeft = false;
}

void Parser::processIdentifiersAndLiterals(bool& processed, bool& error, QStringList& expected, QVector<Token>::const_iterator& token, bool& hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	QString e = expected.isEmpty() ? "" : expected.first();
	if ( e == "expr" || (e == "id" && token->type() == Token::Identifier) )
	{
		instructions.append( new AddValue(token->text()) );
		hasLeft = true;
		expected.removeFirst();
		processed = true;
	}
	else if (e == "id") error = true;
}

void Parser::processOperatorDelimiters(bool& processed, bool& error, QStringList& expected, QVector<Token>::const_iterator& token, bool& hasLeft, ParseResult& result, QVector<ExpressionTreeBuildInstruction*>& instructions)
{
	QString e = expected.isEmpty() ? "" : expected.first();

	if (e == "id")
	{
		error = true;
	}
	else
	{
		bool prefix = e == "expr";

		QList<OperatorDescriptor*> matching_ops;
		if (prefix) {
			matching_ops.append( ops_->findByPrefix(token->text()) );
		}
		else {
			if (!hasLeft)
			{
				// This situation arises for example in the 'delete []' operator where two different operator tokens follow each other.
				processNextExpectedDelimiter(processed, expected, token, hasLeft, result, instructions);
				return;
			}
			matching_ops.append( ops_->findByInfixWithoutPrefix(token->text()) );
			matching_ops.append( ops_->findByPostfixWithoutPreInfix(token->text()) );
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
			QStringList new_expected = oi->signature();
			new_expected.removeFirst(); // Remove the prefix/expr token from the signature
			if (!prefix) new_expected.removeFirst(); // Remove the in/postfix token from the signature
			new_expected.append( "end" );
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

} /* namespace InteractionBase */
