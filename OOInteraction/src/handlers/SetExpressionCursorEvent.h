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

#pragma once

#include "../oointeraction_api.h"

#include "VisualizationBase/src/CustomSceneEvent.h"

namespace Visualization {
	class Item;
}

namespace Model {
	class Node;
}

namespace OOInteraction {

class OOINTERACTION_API SetExpressionCursorEvent : public Visualization::CustomSceneEvent{
	public:
		static const QEvent::Type EventType;
		SetExpressionCursorEvent(Visualization::Item* parentContainer, Model::Node* node, int offset);
		virtual void execute();

	private:

		/**
		 * Instead of just storing the visualization that contains the visualization node, this field stores all
		 * ancestors of that visualization. The reason is that when this cursor event object is constructed, before an
		 * update, a parent container is provided that might turn out to be the wrong container after the update. For
		 * example the expression node, might be recognized as a special expression and its visualization might be
		 * transferred somewhere else.
		 *
		 * Having the entire chain of parents makes it possible to assure that:
		 * - The container that was specified was not deleted.
		 * - The new place where this node is visualized is properly given the cursor.
		 */
		QList<Visualization::Item*> parentContainerChain_;
		Model::Node* node_;
		int offset_;
		bool reposted_{};

		bool tryToSetOffset(Visualization::Item* item);
};

}
