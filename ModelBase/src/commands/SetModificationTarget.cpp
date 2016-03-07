/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "SetModificationTarget.h"

#include "../concurrent/NodeReadWriteLock.h"
#include "../nodes/Node.h"

namespace Model {

SetModificationTarget::SetModificationTarget(Node* &field, NodeReadWriteLock* &lock, QSet<Node*>& modifiedTargets,
															Node* newTarget) :
	UndoCommand{nullptr, "Change modification target"}, field{field}, lock{lock}, modifiedTargets{modifiedTargets},
	oldTarget{field}, newTarget{newTarget}
{
}

void SetModificationTarget::redo()
{
	NodeReadWriteLock* oldLock = nullptr;
	NodeReadWriteLock* newLock = nullptr;

	if (oldTarget) oldLock = oldTarget->accessLock();
	if (newTarget) newLock = newTarget->accessLock();

	// Improve performance by only locking/unlocking if the locks are different
	if (oldLock != newLock)
	{
		if (oldLock) oldLock->unlock();
		if (newLock) newLock->lockForWrite(newTarget);
	}

	field = newTarget;
	if ( newTarget) modifiedTargets.insert(newTarget);

	if (field) lock = field->accessLock();
	else lock = nullptr;

	UndoCommand::redo();
}

void SetModificationTarget::undo()
{
	NodeReadWriteLock* newLock = nullptr;
	NodeReadWriteLock* oldLock = nullptr;

	if (newTarget) newLock = newTarget->accessLock();
	if (oldTarget) oldLock = oldTarget->accessLock();

	// Improve performance by only locking/unlocking if the locks are different
	if (newLock != oldLock)
	{
		if (newLock) newLock->unlock();
		if (oldLock) oldLock->lockForWrite(oldTarget);
	}

	field = oldTarget;
	if ( oldTarget ) modifiedTargets.insert(oldTarget);

	if (field) lock = field->accessLock();
	else lock = nullptr;

	UndoCommand::undo();
}

}
