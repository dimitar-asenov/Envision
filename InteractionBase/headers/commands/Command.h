/***********************************************************************************************************************
 * Command.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMAND_H_
#define COMMAND_H_

#include "interactionbase_api.h"

#include "CommandResult.h"
#include "CommandSuggestion.h"
#include "CommandHelp.h"

#include "VisualizationBase/headers/items/Item.h"

#include <QtCore/QStringList>
#include <QtCore/QList>

namespace Interaction {

class INTERACTIONBASE_API Command
{
	public:
		virtual ~Command();

		virtual bool canInterpret(Visualization::Item* source, Visualization::Item* target, const QStringList& commandTokens);
		virtual CommandResult* execute(Visualization::Item* source, Visualization::Item* target, const QStringList& commandTokens) = 0;

		virtual QList<CommandSuggestion*> suggest(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar);
		virtual QStringList commandForms(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar) = 0;
		virtual QList<CommandHelp*> extendedHelp(Visualization::Item* source, Visualization::Item* target, const QString& commandForm = QString());
};

}

#endif /* COMMAND_H_ */
