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

#pragma once

#include "UndoCommand.h"

namespace Model {

class Node;

class MODELBASE_API NoteNodeChange: public UndoCommand
{
	public:
		NoteNodeChange(QSet<Node*>& modifiedTargets, QSet<Node*>& removedTargets, const UndoCommand* command);
		virtual void redo();
		virtual void undo();

	private:
		QSet<Node*>& modifiedTargets_;
		QSet<Node*>& removedTargets_;
		Node* target_{};
		Node* insertedNode_{};
		Node* removedNode_{};

		void markNodeAndChildrenAsRemoved(Node* node) const;

		/**
		 * It could happen than a subtree which is removed earlier in the undostack, is reinserted at a later time in the
		 * command stack. In such cases we should not report these nodes as removed. (MAYBE)
		 *
		 * TODO: Think about whether we:
		 *  - should not report nodes which have been essentially moved in any way
		 *  - should report moved nodes in some special way
		 *  - should report moved nodes as a pair of deletion/insertion
		 */
		void unmarkRemovals(Node* insertedNode) const;
};

}
