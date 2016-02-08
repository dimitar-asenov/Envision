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

#include "CommandDescriptor.h"

#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/expressions/UnfinishedOperator.h"
#include "OOModel/src/expressions/CommaExpression.h"

namespace OOInteraction {

QMap<QString, CommandExpression*> CommandDescriptor::commands;

bool CommandDescriptor::registerCommand(CommandExpression* command)
{
	if (commands.contains(command->name())) return false;
	else commands.insert(command->name(), command);

	return true;
}

void CommandDescriptor::unregisterCommand(CommandExpression* command)
{
	if (command) commands.remove(command->name());
	SAFE_DELETE(command);
}

CommandDescriptor::CommandDescriptor(const QString& name, const QString& signature,
		int precedence, Associativity associativity)
		: OOOperatorDescriptor{name, signature, precedence, associativity}
{}

CommandDescriptor::~CommandDescriptor()
{
	qDeleteAll(commands);
	commands.clear();
}

OOModel::Expression* CommandDescriptor::create(const QList<OOModel::Expression*>& operands)
{
	auto ref = DCast<OOModel::ReferenceExpression>( operands.first());
	Q_ASSERT(ref);
	QString name = ref->name();
	SAFE_DELETE(ref);

	// Unpack second argument if any
	QList<OOModel::Expression*> arguments;
	if (operands.size() > 1)
	{
		if (auto comma = DCast<OOModel::CommaExpression>(operands.last()))
		{
			for (auto arg : comma->allSubOperands(true)) arguments.append(arg);
			SAFE_DELETE(comma);
		}
		else
			if (!DCast<OOModel::EmptyExpression>(operands.last()) )
				arguments.append(operands.last());
	}

	auto it = commands.find(name);
	OOModel::Expression* e = nullptr;
	if (it != commands.end()) e = (*it)->create(arguments);

	if (e) return e;
	else return createUnfinished(name, arguments);
}

OOModel::UnfinishedOperator* CommandDescriptor::createUnfinished(const QString& name,
		const QList<OOModel::Expression*>& arguments)
{
	auto unf = new OOModel::UnfinishedOperator{};
	unf->delimiters()->append(new Model::Text{"\\"});
	unf->operands()->append(new OOModel::ReferenceExpression{name});

	if (!arguments.isEmpty())
	{
		unf->delimiters()->append(new Model::Text{"("});

		for (int i = 0; i< arguments.size(); ++i)
		{
			if (i > 0) unf->delimiters()->append(new Model::Text{","});
			unf->operands()->append(arguments[i]);
		}

		unf->delimiters()->append(new Model::Text{")"});
	}
	else unf->delimiters()->append(new Model::Text{QString{}}); // append an empty postfix if there are no arguments

	return unf;
}

}
