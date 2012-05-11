/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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
 * CommandExecutionEngine.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/Command.h"
#include "commands/CommandExecutionEngine.h"
#include "commands/CommandResult.h"
#include "commands/CommandSuggestion.h"
#include "handlers/GenericHandler.h"

#include "interactionbase.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/SceneHandlerItem.h"
#include "VisualizationBase/src/Scene.h"

namespace Interaction {

const char* QUOTE_SYMBOLS = "\"'`";
const char* ESCAPE_SYMBOLS = "\\";

CommandExecutionEngine::CommandExecutionEngine() :
	lastCommandResult(nullptr), resultIsExternallyAcquired(false)
{
}

CommandExecutionEngine::~CommandExecutionEngine()
{
	deleteLastCommandResult();
}

CommandResult* CommandExecutionEngine::acquireResult()
{
	resultIsExternallyAcquired = true;
	return lastCommandResult;
}

void CommandExecutionEngine::deleteLastCommandResult()
{
	if (resultIsExternallyAcquired ) lastCommandResult = nullptr;
	else SAFE_DELETE(lastCommandResult);
	resultIsExternallyAcquired = false;
}

CommandExecutionEngine* CommandExecutionEngine::instance()
{
	static CommandExecutionEngine engine;
	return &engine;
}

void CommandExecutionEngine::execute(Visualization::Item *originator, const QString& command)
{
	deleteLastCommandResult();

	QString trimmed = command.trimmed();

	if ( !doQuotesMatch(command, QUOTE_SYMBOLS, ESCAPE_SYMBOLS) )
	{
		lastCommandResult = new CommandResult(new CommandError("A quoted string expands past the end of the command."));
		lastCommandResult->errors().first()->addResolutionTip("Try inserting a matching quote.");
		return;
	}

	QString navigation = extractNavigationString(trimmed);

	// This is the node where we begin trying to process the command
	Visualization::Item* source = originator;
	// Alter the source node according to the requested navigation location.
	if (!navigation.isEmpty()) source = navigate(originator, navigation);

	// This is the node (source or one of its ancestors) where we manage to process the command.
	Visualization::Item* target = source;

	QStringList tokens = tokenize(trimmed, QUOTE_SYMBOLS, ESCAPE_SYMBOLS);

	bool processed = false;

	while(target != nullptr && !processed)
	{
		GenericHandler* handler = dynamic_cast<GenericHandler*> (target->handler());

		if (handler)
		{
			for(int i = 0; i< handler->commands().size(); ++i)
			{
				if ( handler->commands().at(i)->canInterpret(source, target, tokens) )
				{
					lastCommandResult = handler->commands().at(i)->execute(source, target, tokens);

					if (lastCommandResult->code() != CommandResult::CanNotInterpret)
					{
						processed = true;
						break;
					}
					else deleteLastCommandResult();
				}
			}
		}

		if (!processed) target = target->parent();
	}

	// If no item can process this command dispatch it to the SceneItem
	if (!processed)
	{
		GenericHandler* handler = dynamic_cast<GenericHandler*> (source->scene()->sceneHandlerItem()->handler());

		if ( handler )
		{
			for (int i = 0; i < handler->commands().size(); ++i)
			{
				if ( handler->commands().at(i)->canInterpret(source, nullptr, tokens) )
				{
					lastCommandResult = handler->commands().at(i)->execute(source, nullptr, tokens);

					if ( lastCommandResult->code() != CommandResult::CanNotInterpret )
					{
						processed = true;
						break;
					}
					else deleteLastCommandResult();
				}
			}
		}
	}

	// If the command is still not processed this is an error
	if (!processed)
	{
		lastCommandResult = new CommandResult(new CommandError("Unknown command '" + command + "' "));
		InteractionBase::log()->add(Logger::Log::LOGWARNING, "Unknown command: " + command);
	}
}

QList<CommandSuggestion*> CommandExecutionEngine::autoComplete(Visualization::Item *originator, const QString& textSoFar)
{
	QList<CommandSuggestion*> result;

	QString trimmed = textSoFar.trimmed();
	QString navigation = extractNavigationString(trimmed);

	if (!trimmed.isEmpty())
	{
		// This is the node where we begin trying to process the command
		Visualization::Item* source = originator;
		// Alter the source node according to the requested navigation location.
		if (!navigation.isEmpty()) source = navigate(originator, navigation);
		// This is the node (source or one of its ancestors) where we manage to process the command.
		Visualization::Item* target = source;

		// Get suggestion from item and parents
		while(target != nullptr)
		{
			GenericHandler* handler = dynamic_cast<GenericHandler*> (target->handler());
			if (handler)
				for(int i = 0; i< handler->commands().size(); ++i)
					result.append( handler->commands().at(i)->suggest(source, target, trimmed) );

			target = target->parent();
		}

		// Get suggestions from the scene handler item
		GenericHandler* handler = dynamic_cast<GenericHandler*> (source->scene()->sceneHandlerItem()->handler());
		if ( handler )
			for (int i = 0; i < handler->commands().size(); ++i)
				result.append( handler->commands().at(i)->suggest(source, target, trimmed) );
	}

	return result;
}

QString CommandExecutionEngine::extractNavigationString(QString& command)
{
	// Extract navigation information if any
	QString trimmed = command.trimmed();
	QString simplified = trimmed.simplified(); // Note that this will destroy any spacing within quotations.
	QString navigation;

	if ( simplified.startsWith("~ ") || simplified.startsWith(".. ") || simplified.startsWith(". ") || simplified.startsWith("../")
			|| simplified.startsWith("./") || simplified.startsWith("/") )
	{
		navigation = trimmed.left(simplified.indexOf(' '));
		command = trimmed.mid(navigation.size()).trimmed();
	}

	return navigation;
}

Visualization::Item* CommandExecutionEngine::navigate(Visualization::Item *originator, const QString&)
{
	//TODO figure out what navigation we want and implement it.
	return originator;
}

QStringList CommandExecutionEngine::tokenize(const QString& string, const QString& quoteSymbols, const QString& escapeSymbols)
{
	QStringList result;
	QString str;

	QChar quote;

	for (int i = 0; i < string.size(); ++i)
	{
		if ( quote.isNull() )
		{
			if ( quoteSymbols.contains(string[i]) )
			{
				if ( !str.isNull() ) result.append(tokenizeNonQuoted(str.simplified()));
				quote = string[i];
				str = quote;
			}
			else str.append(string[i]);
		}
		else
		{
			if ( string[i] == quote && !isEscaped(string, i, escapeSymbols) )
			{
				result.append(str + quote);
				quote = QChar();
				str = QString();
			}
		}
	}

	if ( !str.isNull() ) result.append(tokenizeNonQuoted(str.simplified()));
	return result;
}

QStringList CommandExecutionEngine::tokenizeNonQuoted(const QString& string)
{
	//TODO The concept of tokens here is unclear. Define this better.
	QStringList result;
	QString str;

	QChar last;
	bool fractionInitiated = false;

	for (int i = 0; i < string.size(); ++i)
	{
		if ( string[i] == ' ' )
		{
			if ( !str.isNull() ) result.append(str);
			str = QString();
			fractionInitiated = false;
		}
		else if ( string[i].isDigit() && fractionInitiated && last == '.' )
		{
			str = result.last() + '.' + string[i];
			result.removeLast();
			fractionInitiated = false;
		}
		else if ( string[i].isDigit() && str.isNull() )
		{
			fractionInitiated = true;
			str += string[i];
		}
		else if ( string[i] == '.' && fractionInitiated )
		{
			result.append(str);
			str = ".";
		}
		else if ( string[i].isDigit() && fractionInitiated )
		{
			str += string[i];
		}
		else if ( (string[i].isLetter() || string[i].isDigit() || string[i] == '_') != (last.isLetter() || last.isDigit() || last == '_') )
		{
			if ( !str.isNull() ) result.append(str);
			str = string[i];
			fractionInitiated = false;
		}
		else
		{
			str += string[i];
		}

		last = string[i];
	}

	if ( !str.isNull() ) result.append(str);
	return result;
}

bool CommandExecutionEngine::doQuotesMatch(const QString& string, const QString& quoteSymbols, const QString& escapeSymbols)
{
	QChar quote;

	for (int i = 0; i < string.size(); ++i)
	{
		if ( quote.isNull() )
		{
			if ( quoteSymbols.contains(string[i]) ) quote = string[i];
		}
		else
		{
			if ( string[i] == quote && !isEscaped(string, i, escapeSymbols) ) quote = QChar();
		}
	}

	return quote.isNull();
}

bool CommandExecutionEngine::isEscaped(const QString& string, int indexToCheck, const QString& escapeSymbols)
{
	int num = 0;
	int index = indexToCheck - 1;
	while ( index >= 0 && escapeSymbols.contains(string.at(index)) )
	{
		--index;
		++num;
	}

	return num % 2 == 1;
}

}
