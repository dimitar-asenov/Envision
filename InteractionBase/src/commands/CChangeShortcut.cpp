/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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

#include "CChangeShortcut.h"
#include "../input_actions/ActionRegistry.h"


namespace Interaction {

CChangeShortcut::CChangeShortcut() : Command{"changeShortcut"}{}

bool CChangeShortcut::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() > 1 && commandTokens.first() == name();
}

CommandResult* CChangeShortcut::execute(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	if (commandTokens.size() < 2)
		return new CommandResult(new CommandError("Please specify a shortcut to change"));

	QString shortcutName;
	for (auto part : commandTokens.mid(1))
		shortcutName += part;
    ActionRegistry::instance()->enterChangeShortcutState(shortcutName);
	return new CommandResult();
}

QList<CommandSuggestion*> CChangeShortcut::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QList<CommandSuggestion*> results;
	if (textSoFar.trimmed().startsWith("changeShortcut ", Qt::CaseInsensitive))
	{
		auto name = textSoFar.trimmed().mid(15);
        for (auto eventName : ActionRegistry::instance()->inputHandlers())
			if (eventName.startsWith(name))
				results.append(new CommandSuggestion("changeShortcut " + eventName,
					"Change shortcut of " + eventName));
	}
	else if (name().startsWith(textSoFar.trimmed()))
		results.append(new CommandSuggestion("changeShortcut ", "Change a shortcut"));
	return results;
}

}
