/***********************************************************************************************************************
 * GenericHandler.h
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef GENERICHANDLER_H_
#define GENERICHANDLER_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/InteractionHandler.h"

namespace Interaction {

class Command;
class CommandExecutionEngine;

class INTERACTIONBASE_API GenericHandler : public Visualization::InteractionHandler
{
	public:
		static GenericHandler* instance();

		const QList<Command*>& commands();
		void addCommand(Command* command);

		void setCommandExecutionEngine(CommandExecutionEngine *engine);

		// Mouse events
		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);

		// Command events
		virtual void command(Visualization::Item *target, const QString& command);

	protected:
		GenericHandler();

	private:
		QList<Command*> supportedCommands;
		CommandExecutionEngine* executionEngine;
};

inline const QList<Command*>& GenericHandler::commands() { return supportedCommands; }
inline void GenericHandler::addCommand(Command* command) { supportedCommands.append(command); }
inline void GenericHandler::setCommandExecutionEngine(CommandExecutionEngine *engine) { executionEngine = engine; };

}

#endif /* GENERICHANDLER_H_ */
