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

bool Command::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList&)
{
	return true;
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
