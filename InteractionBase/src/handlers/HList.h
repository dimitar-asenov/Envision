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

#pragma once

#include "../interactionbase_api.h"

#include "GenericHandler.h"
#include "../events/SetCursorEvent.h"

namespace Model {
	class Node;
}

namespace Visualization {
	class VList;
}

namespace Interaction {

class INTERACTIONBASE_API HList : public GenericHandler
{
	public:
		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event) override;


		virtual void scheduleSetCursor(Visualization::VList* list, Model::Node* listNodeToSelect,
				SetCursorEvent::CursorPlacement howToSelectItem);

		virtual void scheduleSetCursor(Visualization::VList* list, int setCursorNodeIndex);

		/**
		 * Removes the node at position \a removeAtNodeIndex from \a list and sets the cursor inside the element below
		 * or above the removed one according to \a setCursorDown and \a howToSelectItem
		 */
		virtual void removeNodeAndSetCursor(Visualization::VList* list, int removeAtNodeIndex, bool setCursorDown,
				SetCursorEvent::CursorPlacement howToSelectItem);
		void removeNodeAndSetCursor(Visualization::VList* list, Model::Node* removeNode, bool setCursorDown,
				SetCursorEvent::CursorPlacement howToSelectItem);

		/**
		 * Removes the node at position \a removeAtNodeIndex from \a list and sets the cursor to the list cursor below
		 * or above the removed element as indicated by \a setCursorDown
		 */
		virtual void removeNodeAndSetCursor(Visualization::VList* list, int removeAtNodeIndex);
		void removeNodeAndSetCursor(Visualization::VList* list, Model::Node* removeNode);

		static HList* instance();

	protected:
		HList();
};

}
