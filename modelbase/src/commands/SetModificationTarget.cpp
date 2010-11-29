/***********************************************************************************************************************
 * SetModificationTarget.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "SetModificationTarget.h"
#include "nodes/Node.h"
#include "NodeReadWriteLock.h"

namespace Model {

SetModificationTarget::SetModificationTarget(Node* &field_, NodeReadWriteLock* &lock_, Node* oldTarget_, Node* newTarget_) :
	UndoCommand(NULL, "Change modification target"), field(field_), lock(lock_), oldTarget(oldTarget_), newTarget(newTarget_)
{
}

void SetModificationTarget::redo()
{
	if (oldTarget) oldTarget->getAccessLock()->unlock();
	if (newTarget) newTarget->getAccessLock()->lockForWrite(newTarget);

	field = newTarget;
	if (field) lock = field->getAccessLock();
	else lock = NULL;

	UndoCommand::redo();
}

void SetModificationTarget::undo()
{
	if (newTarget) newTarget->getAccessLock()->unlock();
	if (oldTarget) oldTarget->getAccessLock()->lockForWrite(oldTarget);

	field = oldTarget;
	if (field) lock = field->getAccessLock();
	else lock = NULL;

	UndoCommand::undo();
}

}
