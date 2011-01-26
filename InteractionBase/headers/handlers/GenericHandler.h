/***********************************************************************************************************************
 * GenericHandler.h
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef GENERICHANDLER_H_
#define GENERICHANDLER_H_

#include "../interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/InteractionHandler.h"

#include <QtCore/QList>

namespace Model { class Node; }

namespace Interaction {

class Command;
class CommandExecutionEngine;
class CommandPrompt;

class INTERACTIONBASE_API GenericHandler : public Visualization::InteractionHandler
{
	public:
		enum FocusDirection {NOT_SPECIFIED, FROM_TOP, FROM_BOTTOM, FROM_LEFT, FROM_RIGHT};

		static GenericHandler* instance();

		static CommandExecutionEngine* executionEngine();
		static void setCommandExecutionEngine(CommandExecutionEngine *engine);

		static void setFocusDirection(FocusDirection direction);
		static FocusDirection focusDirection();

		/**
		 * TODO: Commands are not inherited when the handler is inherited. Is this right?
		 */
		const QList<Command*>& commands();
		void addCommand(Command* command);

		CommandPrompt* prompt();
		void removeCommandPrompt();
		void showCommandPrompt(Visualization::Item* commandRecevier);

		virtual void beforeEvent(Visualization::Item *target, QEvent* event);

		// Keyboard events
		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);
		virtual void keyReleaseEvent(Visualization::Item *target, QKeyEvent *event);

		// Mouse events
		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);

		// Command events
		virtual void command(Visualization::Item *target, const QString& command);

	protected:
		GenericHandler();

		virtual void filterSelectedItems(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void arrangeNodesForClipboard(QList<const Model::Node*>& list);

	private:
		QList<Command*> supportedCommands;

		static CommandExecutionEngine* executionEngine_;
		static CommandPrompt* prompt_;
		static FocusDirection focusDirection_;


};

inline const QList<Command*>& GenericHandler::commands() { return supportedCommands; }
inline void GenericHandler::addCommand(Command* command) { supportedCommands.append(command); }

}

#endif /* GENERICHANDLER_H_ */
