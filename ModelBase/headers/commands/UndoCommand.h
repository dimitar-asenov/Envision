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
