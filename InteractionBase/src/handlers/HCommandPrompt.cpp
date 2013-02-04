/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

/***********************************************************************************************************************
 * HCommandPrompt.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HCommandPrompt.h"

#include "vis/CommandPrompt.h"
#include "commands/CommandExecutionEngine.h"
#include "commands/CommandResult.h"

#include "VisualizationBase/src/Scene.h"

namespace Interaction {

HCommandPrompt::HCommandPrompt()
{
}

HCommandPrompt* HCommandPrompt::instance()
{
	static HCommandPrompt h;
	return &h;
}

void HCommandPrompt::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	CommandPrompt* prompt = static_cast<CommandPrompt*> (target);

	if (event->key() == Qt::Key_Escape)
	{
		prompt->hidePrompt();
	}
	else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
	{
		prompt->commandReceiver()->execute(prompt->text());

		CommandResult* result = executionEngine()->acquireResult();

		if ( result->code() == CommandResult::OK)
		{
			prompt->hidePrompt();
		}
		else
		{
			prompt->setResult(result);
		}
	}
	else if (event->key() == Qt::Key_Tab)
	{
		int numSuggestions = prompt->suggestions().size();
		if (prompt->result()) numSuggestions += prompt->result()->suggestions().size();

		if ( numSuggestions  == 1)
		{
			if ( prompt->suggestions().size() == 1 ) prompt->takeSuggestion(prompt->suggestions().first());
			else prompt->takeSuggestion(prompt->result()->suggestions().first());
		}
	}
	else GenericHandler::keyPressEvent(target, event);
}

}
