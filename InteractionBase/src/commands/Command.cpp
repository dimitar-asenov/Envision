/***********************************************************************************************************************
 * Command.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/Command.h"

namespace Interaction {

Command::~Command()
{
}

Command::InterpretationPossible Command::canInterpret(Visualization::Item*, Visualization::Item*, const QString&)
{
	return DontKnow;
}

QList<CommandSuggestion*> Command::suggest(Visualization::Item*, Visualization::Item*, const QString&)
{
	return QList<CommandSuggestion*>();
}

QList<CommandHelp*> Command::extendedHelp(Visualization::Item*, Visualization::Item*, const QString&)
{
	return QList<CommandHelp*>();
}

}
