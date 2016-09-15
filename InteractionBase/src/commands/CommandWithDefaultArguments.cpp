/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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

#include "CommandWithDefaultArguments.h"

namespace Interaction {

CommandWithDefaultArguments::CommandWithDefaultArguments(QString name, const QStringList &defaultArguments)
	 :Command{name, true}, defaultArguments_{defaultArguments}
{}

bool CommandWithDefaultArguments::canInterpret(Visualization::Item *, Visualization::Item *,
	const QStringList &commandTokens, const std::unique_ptr<Visualization::Cursor> &)
{
	 return !commandTokens.isEmpty() && commandTokens.first() == this->name();
}

CommandResult* CommandWithDefaultArguments::execute(Visualization::Item *source, Visualization::Item *target,
	const QStringList &commandTokens, const std::unique_ptr<Visualization::Cursor> &cursor)
{
	 return this->executeWithArguments(source, target, getParameters(commandTokens), cursor);
}

QList<CommandSuggestion*> CommandWithDefaultArguments::suggest(Visualization::Item *source, Visualization::Item *target,
	const QString &textSoFar, const std::unique_ptr<Visualization::Cursor> &cursor)
{
	QList<CommandSuggestion*> result;

	if (!textSoFar.isEmpty() &&
		 ( textSoFar.trimmed().startsWith(this->name() + " ") || this->name().startsWith(textSoFar.trimmed())))
	{
		auto parameters = getParameters(textSoFar.split(" "));
		parameters.prepend(name());
		if (canInterpret(source, target, parameters, cursor))
			result.append(new CommandSuggestion{this->name(),
				this->description(source, target, getParameters(textSoFar.split(" ")), cursor)});
	}

	return result;
}

QStringList CommandWithDefaultArguments::getParameters(const QStringList &commandTokens)
{
	QStringList parameters = commandTokens;

	// Remove the command name itself
	parameters.removeAt(0);

	// Fill up with default arguments
	for (int i = parameters.size(); i < defaultArguments_.size(); i++)
		parameters.append(defaultArguments_[i]);
	return parameters;
}

}
