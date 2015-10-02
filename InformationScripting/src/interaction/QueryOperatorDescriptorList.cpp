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

#include "QueryOperatorDescriptorList.h"

#include "QueryOperatorDescriptor.h"
#include "../nodes/OperatorQueryNode.h"
#include "../nodes/CommandNode.h"

namespace InformationScripting {

QueryOperatorDescriptorList* QueryOperatorDescriptorList::instance()
{
	static QueryOperatorDescriptorList instance;
	return &instance;
}

void QueryOperatorDescriptorList::initializeWithDefaultOperators()
{
	using QOD = QueryOperatorDescriptor;
	using OpType = OperatorQueryNode::OperatorTypes;
	add(new QOD("pipe operator", "expr | expr", 2, QOD::LeftAssociative, QOD::operatorQuery<OpType::Pipe>));
	add(new QOD("pipe operator", "expr - expr", 2, QOD::LeftAssociative, QOD::operatorQuery<OpType::Substract>));
	add(new QOD("pipe operator", "expr U expr", 2, QOD::LeftAssociative, QOD::operatorQuery<OpType::Union>));

	add(new QOD("command with arg", "id SPACE expr", 1, QOD::LeftAssociative,
					[](const QList<QueryNode*>& operands) -> QueryNode* {
		auto cmd = DCast<CommandNode>(operands.first());
		if (!cmd) return operands.first();

		for (int i = 1; i < operands.size(); ++i)
		{
			qDebug() << operands[i]->typeName();
			cmd->arguments()->append(operands[i]);
		}
		return cmd;
	}));

	// TODO how to represent arguments?? Maybe use unary minus (-).
	// But then what to do with an eventual = ??
	// Arguments often have the form: -argName=someValue or --argumentName=someValue
	// Where the assignment is optional.
}

} /* namespace InformationScripting */
