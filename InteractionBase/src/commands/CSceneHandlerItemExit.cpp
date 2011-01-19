/***********************************************************************************************************************
 * CSceneHandlerItemExit.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/CSceneHandlerItemExit.h"

#include "VisualizationBase/headers/VisualizationManager.h"

namespace Interaction {

bool CSceneHandlerItemExit::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens)
{
	return commandTokens.size() == 1 && commandTokens.first() == "exit";
}

CommandResult* CSceneHandlerItemExit::execute(Visualization::Item*, Visualization::Item*, const QStringList&)
{
	Visualization::VisualizationManager::instance().getMainWindow()->close();

	return new CommandResult();
}

QList<CommandSuggestion*> CSceneHandlerItemExit::suggest(Visualization::Item*, Visualization::Item*, const QString& textSoFar)
{
	QList<CommandSuggestion*> s;
	if (QString("exit").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
			s.append(new CommandSuggestion("exit", "Closes Envision"));
	return s;
}

QStringList CSceneHandlerItemExit::commandForms(Visualization::Item*, Visualization::Item*, const QString& textSoFar)
{
	QStringList forms;
	if (textSoFar.isEmpty() || QString("exit").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
		forms.append("exit");
	return forms;
}

}
