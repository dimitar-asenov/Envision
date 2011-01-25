/***********************************************************************************************************************
 * UndoCommand.h
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef UNDOCOMMAND_H_
#define UNDOCOMMAND_H_

#include "../modelbase_api.h"
#include <QtGui/QUndoCommand>

namespace Model {

class Node;

class MODELBASE_API UndoCommand: public QUndoCommand
{
	private:
		Node* target_;
		bool undone;

	public:
		UndoCommand(Node* target, const QString & text);
		virtual ~UndoCommand();

		virtual void redo();
		virtual void undo();

		/**
		 * Returns whether the command has been undone since it was applied.
		 *
		 * If a command is undone after it was originally executed, this will return true. If a command which was undone
		 * is redone this will return false.
		 *
		 * This is used for memory management. If a command is destroyed (leaves the undo stack) it may need to delete
		 * objects associated with it. A command can leave the stack for two reasons:
		 * - It is at the bottom of the stack and the stack is full. Therefore this command needs to be deleted to free up
		 * space for new commands. In this case isUndone will return false.
		 * - The command has been undone and a new command has been pushed on top of the stack. In this case the current
		 * command is deleted and isUndone is true.
		 */
		bool isUndone();

		Node* target();
};

inline bool UndoCommand::isUndone() { return undone; }
inline Node* UndoCommand::target() { return target_; }

}

#endif /* UNDOCOMMAND_H_ */
