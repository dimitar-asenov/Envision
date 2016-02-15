/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "NodeOwningCommand.h"
#include "../nodes/Node.h"
#include "../model/AllTreeManagers.h"
#include "../model/TreeManager.h"

namespace Model {

NodeOwningCommand::NodeOwningCommand(Node* target, const QString & text, Node* ownedIfDone, Node* ownedIfUndone)
: UndoCommand{target, text}, ownedIfDone_{ownedIfDone}, ownedIfUndone_{ownedIfUndone}
{
	// If the target node is not yet owned, do not assume ownership over its subnodes.
	if (target->manager() == nullptr)
	{
		auto managerOfOwnedIfDone = ownedIfDone_ ?
			AllTreeManagers::instance().managerOfOwningUndoStack(ownedIfDone_) : nullptr;
		if (!managerOfOwnedIfDone) ownedIfDone_ = nullptr;

		auto managerOfOwnedIfUnDone = ownedIfUndone_ ?
			AllTreeManagers::instance().managerOfOwningUndoStack(ownedIfUndone_) : nullptr;
		if (!managerOfOwnedIfUnDone) ownedIfUndone_ = nullptr;
	}
}

NodeOwningCommand::~NodeOwningCommand()
{
	auto n = owned();
	// Only delete a node if:
	// - It is not part of a manager
	// - It is not currently owned by any other command in any undo stack
	if (n && !n->manager())
		if (AllTreeManagers::instance().managerOfOwningUndoStack(n, this) == nullptr)
			SAFE_DELETE(n);
}

Node* NodeOwningCommand::owned() const
{
	return isUndone() ? ownedIfUndone_ : ownedIfDone_;
}

Node* NodeOwningCommand::insertedNode() const
{
	return ownedIfUndone_;
}

Node* NodeOwningCommand::removedNode() const
{
	return ownedIfDone_;
}

}
