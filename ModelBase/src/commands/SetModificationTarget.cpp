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

SetModificationTarget::SetModificationTarget(Node* &field_, NodeReadWriteLock* &lock_, QList<Node*>& modifiedTargets_, Node* newTarget_) :
	UndoCommand(NULL, "Change modification target"), field(field_), lock(lock_), modifiedTargets(modifiedTargets_), oldTarget(field_), newTarget(newTarget_)
{
}

void SetModificationTarget::redo()
{
	if (oldTarget) oldTarget->accessLock()->unlock();
	if (newTarget) newTarget->accessLock()->lockForWrite(newTarget);

	field = newTarget;
	if ( newTarget && !modifiedTargets.contains(newTarget) ) modifiedTargets.append(newTarget);

	if (field) lock = field->accessLock();
	else lock = NULL;

	UndoCommand::redo();
}

void SetModificationTarget::undo()
{
	if (newTarget) newTarget->accessLock()->unlock();
	if (oldTarget) oldTarget->accessLock()->lockForWrite(oldTarget);

	field = oldTarget;
	if ( oldTarget && !modifiedTargets.contains(oldTarget) ) modifiedTargets.append(oldTarget);

	if (field) lock = field->accessLock();
	else lock = NULL;

	UndoCommand::undo();
}

}
