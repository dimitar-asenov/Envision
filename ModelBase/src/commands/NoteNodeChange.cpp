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

#include "NoteNodeChange.h"
#include "../nodes/Node.h"

namespace Model {

NoteNodeChange::NoteNodeChange(QSet<Node*>& modifiedTargets, QSet<Node*>& removedTargets, const UndoCommand* command)
	: UndoCommand{nullptr, "Note node changes"}, modifiedTargets_{modifiedTargets}, removedTargets_{removedTargets},
	  target_{command->target()}, insertedNode_{command->insertedNode()}, removedNode_{command->removedNode()}
{
}

void NoteNodeChange::redo()
{
	if ( target_ ) modifiedTargets_.insert(target_);
	if ( insertedNode_ ) unmarkRemovals(insertedNode_);
	if ( removedNode_  ) markNodeAndChildrenAsRemoved( removedNode_ );
	UndoCommand::redo();
}

void NoteNodeChange::undo()
{
	if ( target_ ) modifiedTargets_.insert(target_);
	if ( removedNode_ ) unmarkRemovals(removedNode_);
	if ( insertedNode_ ) markNodeAndChildrenAsRemoved( insertedNode_ );
	UndoCommand::undo();
}

void NoteNodeChange::markNodeAndChildrenAsRemoved(Node* node) const
{
	QList<Node*> stack {node};
	while (!stack.isEmpty())
	{
		Node* child = stack.takeLast();
		removedTargets_.insert(child);
		stack.append( child->children() );
	}
}

void NoteNodeChange::unmarkRemovals(Node*) const
{
}

}
