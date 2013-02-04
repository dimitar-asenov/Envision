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

/***********************************************************************************************************************
 * SetModificationTarget.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "SetModificationTarget.h"
#include "nodes/Node.h"
#include "../concurrent/NodeReadWriteLock.h"

namespace Model {

SetModificationTarget::SetModificationTarget(Node* &field_, NodeReadWriteLock* &lock_, QList<Node*>& modifiedTargets_, Node* newTarget_) :
	UndoCommand(nullptr, "Change modification target"), field(field_), lock(lock_), modifiedTargets(modifiedTargets_), oldTarget(field_), newTarget(newTarget_)
{
}

void SetModificationTarget::redo()
{
	if (oldTarget) oldTarget->accessLock()->unlock();
	if (newTarget) newTarget->accessLock()->lockForWrite(newTarget);

	field = newTarget;
	if ( newTarget && !modifiedTargets.contains(newTarget) ) modifiedTargets.append(newTarget);

	if (field) lock = field->accessLock();
	else lock = nullptr;

	UndoCommand::redo();
}

void SetModificationTarget::undo()
{
	if (newTarget) newTarget->accessLock()->unlock();
	if (oldTarget) oldTarget->accessLock()->lockForWrite(oldTarget);

	field = oldTarget;
	if ( oldTarget && !modifiedTargets.contains(oldTarget) ) modifiedTargets.append(oldTarget);

	if (field) lock = field->accessLock();
	else lock = nullptr;

	UndoCommand::undo();
}

}
