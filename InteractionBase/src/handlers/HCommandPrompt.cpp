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

#include "VisualizationBase/headers/Scene.h"

namespace Interaction {

HCommandPrompt::HCommandPrompt()
{
}

HCommandPrompt* HCommandPrompt::instance()
{
	static HCommandPrompt h;
	return &h;
}

void HCommandPrompt::keyReleaseEvent(Visualization::Item *target, QKeyEvent *event)
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

		}
		else
		{
			prompt->setResult(result);
			prompt->scene()->updateTopLevelItems();
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
			prompt->scene()->updateTopLevelItems();
		}
	}
	else GenericHandler::keyReleaseEvent(target, event);
}

}
