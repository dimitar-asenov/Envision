/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#pragma once

#include "../interactionbase_api.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/InteractionHandler.h"

namespace Model {
	class Node;
	class Model;
}

namespace Interaction {

class INTERACTIONBASE_API GenericHandlerModelListener : public QObject
{
	Q_OBJECT

	private:
		// This is needed in order to make the Signals and Slots mechanism work. Otherwise we are not able to connect to
		// the signal provided from Model. This is because the signatures of the two methods, must match exactly
		// (stringwise).
		using Node = Model::Node;

	public:
		void listenToModelOf(Visualization::Item* item);
		void stopListeningToModelOf(Visualization::Item* item);

	public slots:
		void nodesUpdated(QList<Node*> nodes);

	private:
		Model::Model* modelOf(Visualization::Item* item);
		QList<Model::Model*> models_{};
};

class Command;
class CommandExecutionEngine;
class CommandPrompt;
class ActionPrompt;

class INTERACTIONBASE_API GenericHandler : public Visualization::InteractionHandler
{
	public:
		enum CursorMoveOrientation {NoOrientation, VerticalOrientation, HorizontalOrientation};

		static GenericHandler* instance();

		static CommandExecutionEngine* executionEngine();
		static void setCommandExecutionEngine(CommandExecutionEngine *engine);

		/**
		 * TODO: Commands are not inherited when the handler is inherited. Is this right?
		 */
		const QList<Command*>& commands();
		void addCommand(Command* command);

		CommandPrompt* commandPrompt();
		void removeCommandPrompt();
		void showCommandPrompt(Visualization::Item* commandRecevier);

		virtual void beforeEvent(Visualization::Item *target, QEvent* event);

		// Keyboard events
		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);

		// Mouse events
		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);

		// Focus events
		virtual void focusInEvent(Visualization::Item *target, QFocusEvent *event);

		// Command events
		virtual void command(Visualization::Item *target, const QString& command);

		/**
		 * Removes the node visualized by \a target from the list which contains it.
		 *
		 * Returns true if \a target has a parent of type VList and its list node contains the target node. Otherwise this
		 * method does nothing and returns false.
		 */
		bool removeFromList(Visualization::Item* target);

		ActionPrompt* actionPrompt();
		void removeActionPrompt();
		void showActionPrompt(Visualization::Item *actionRecevier, bool autoExecuteAction);
		virtual void action(Visualization::Item *target, const QString& action);

		static void fixCursorPositionForUndoAfterModelChange();

	protected:
		GenericHandler();

		virtual void filterSelectedItems(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void arrangeNodesForClipboard(QList<const Model::Node*>& list);

		bool moveCursor(Visualization::Item *target, int key);

	private:
		QList<Command*> supportedCommands;

		static CommandExecutionEngine* executionEngine_;
		static CommandPrompt* commandPrompt_;
		static ActionPrompt* actionPrompt_;

		static QPoint cursorOriginMidPoint_;
		static CursorMoveOrientation cursorMoveOrientation_;

		static void resetCursorOrigin();

		// The item in the pair below should be a top-level item.
		// The cursor's position should be in terms of the top-level item coordinates.
		static QList<QPair<Visualization::Item*, QPoint> > cursorPositionsForUndo_;
		static QPair<Visualization::Item*, QPoint> lastCursorPosition_;
		static int cursorUndoIndex_;
		static void recordCursorPosition(Visualization::Item* target);
		static GenericHandlerModelListener& modelListener();
};

inline const QList<Command*>& GenericHandler::commands() { return supportedCommands; }
inline void GenericHandler::addCommand(Command* command) { supportedCommands.append(command); }

}
