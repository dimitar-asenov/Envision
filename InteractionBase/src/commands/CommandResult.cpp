/***********************************************************************************************************************
 * CommandResult.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/CommandResult.h"

#include "Core/headers/global.h"

namespace Interaction {

CommandResult::CommandResult(Code resultCode) :
	outcomeCode(resultCode)
{
}

CommandResult::CommandResult(CommandError* error) :
	outcomeCode(Error)
{
	errors_.append(error);
}

CommandResult::CommandResult(CommandError* error, CommandSuggestion* suggestion) :
	outcomeCode(Error)
{
	errors_.append(error);
	suggestions_.append(suggestion);
}

CommandResult::~CommandResult()
{
	for (int i = 0; i<errors_.size(); ++i) SAFE_DELETE(errors_[i]);
	for (int i = 0; i<suggestions_.size(); ++i) SAFE_DELETE(suggestions_[i]);
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
