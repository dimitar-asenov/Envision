/***********************************************************************************************************************
 * SetModificationTarget.h
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SETMODIFICATIONTARGET_H_
#define SETMODIFICATIONTARGET_H_

#include "UndoCommand.h"

namespace Model {

class NodeReadWriteLock;

class SetModificationTarget: public UndoCommand
{
	private:
		Node* &field;
		NodeReadWriteLock* &lock;

		Node* oldTarget;
		Node* newTarget;

	public:
		SetModificationTarget(Node* &field, NodeReadWriteLock* &lock, Node* oldTarget, Node* newTarget);
		void redo();
		void undo();
};

}

#endif /* SETMODIFICATIONTARGET_H_ */
