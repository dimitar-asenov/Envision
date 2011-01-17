/***********************************************************************************************************************
 * CommandResult.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/CommandResult.h"

namespace Interaction {

CommandResult::CommandResult(Code resultCode) :
	outcomeCode(resultCode)
{
}

CommandResult::~CommandResult()
{
}

void CommandResult::setCode(Code code)
{
	outcomeCode = code;
}

void CommandResult::addError(CommandError* error)
{
	errors_.append(error);
}

void CommandResult::addSuggestion(CommandSuggestion* suggestion)
{
	suggestions_.append(suggestion);
}

}
