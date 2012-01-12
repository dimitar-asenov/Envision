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
 * Parser.h
 *
 *  Created on: Jan 11, 2012
 *      Author: Dimitar Asenov
 */

#ifndef INTERACTIONBASE_PARSER_H_
#define INTERACTIONBASE_PARSER_H_

#include "interactionbase_api.h"

#include "Token.h"
#include "ParseResult.h"

namespace Interaction {

class OperatorDescriptorList;
class ExpressionTreeBuildInstruction;

class INTERACTIONBASE_API Parser {
	public:
		Parser(const OperatorDescriptorList* ops);
		QVector<ExpressionTreeBuildInstruction*> parse(QVector<Token> tokens);

	private:
		ParseResult parse(QVector<Token>::const_iterator token, ParseResult result, QStringList& expected, bool hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions);

		// Returns the finished expression
		QString getNextExpectedDelimiter(const QStringList& expected, int& index);

		// This method will advance the current operator if the next expected delimtier is seen. This is not always correct for example in this case: "template < a>b > ...".
		// TODO: Fix the problem described above.
		void processNextExpectedDelimiter(bool& processed, QStringList& expected, QVector<Token>::const_iterator& token, bool& hasLeft, ParseResult& result, QVector<ExpressionTreeBuildInstruction*>& instructions);
		void processIdentifiersAndLiterals(bool& processed, bool& error, QStringList& expected, QVector<Token>::const_iterator& token, bool& hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions);
		void processOperatorDelimiters(bool& processed, bool& error, QStringList& expected, QVector<Token>::const_iterator& token, bool& hasLeft, ParseResult& result, QVector<ExpressionTreeBuildInstruction*>& instructions);

		QVector<Token>::const_iterator end_tokens_;
		const OperatorDescriptorList* ops_;
};

} /* namespace InteractionBase */
#endif /* INTERACTIONBASE_PARSER_H_ */
