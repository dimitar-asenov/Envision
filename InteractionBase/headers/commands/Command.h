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

#include <QtCore/QList>

namespace Interaction {

class INTERACTIONBASE_API Command
{
	public:
		enum InterpretationPossible {No, Yes, DontKnow};

		virtual ~Command();

		virtual InterpretationPossible canInterpret(Visualization::Item* source, Visualization::Item* target, const QString& command);
		virtual CommandResult execute(Visualization::Item* source, Visualization::Item* target, const QString& command) = 0;

		virtual QList<CommandSuggestion*> suggest(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar);
		virtual QList<QString> commandForms(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar) = 0;
		virtual QList<CommandHelp*> extendedHelp(Visualization::Item* source, Visualization::Item* target, const QString& commandForm = QString());
};

}

#endif /* COMMAND_H_ */
