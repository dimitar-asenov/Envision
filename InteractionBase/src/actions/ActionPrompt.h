/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "interactionbase_api.h"
#include "ActionPromptStyle.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"

namespace Interaction {
class Action;

class INTERACTIONBASE_API ActionPrompt : public Super<Visualization::Item>
{
	ITEM_COMMON(ActionPrompt)

	public:
		ActionPrompt(Visualization::Item* actionReceiver, bool autoExecuteAction,
				const StyleType* style = itemStyles().get());
		virtual ~ActionPrompt();

		Visualization::Item* originalActionReceiver();
		Visualization::Item* currentActionReceiver();

		int numActions();
		bool autoExecuteAction() const;

		Visualization::Text* text();

		void showPrompt();
		void hidePrompt();

		void upParentActionsLevel();
		void downParentActionsLevel();

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:

		bool autoExecuteAction_;
		Visualization::Item* originalActionReceiver_;
		Visualization::Item* currentActionReceiver_;
		Visualization::SequentialLayout* layout_;
		Visualization::SequentialLayout* actionsContainer_;
		Visualization::Text* actionText_;

		QPoint receiverCursorPosition_;

		/**
		 * The parentActionsLevel_ is used to indicate which item exactly should be used as a source for available
		 * actions.
		 *
		 * When the action prompt is created the value of parentActionsLevel_ is 0. This means that a prompt should
		 * appear for the first parent of actionReceiver_, whose node contains actions. If parentActionsLevel_ is more
		 * than 0, then that many items in actionReceiver_'s hierarchy will be skipped. Only items that actually provide
		 * actions are considered.
		 *
		 * Negative values behave similarly for child items. Child items are explored in breadth first manner.
		 *
		 * This is used to quickly navigate to actions related to the current cursor position.
		 */
		int parentActionsLevel_{0};

		void acquireCursor();
		void setPromptPosition();
		void computeCurrentActionReceiver();
		QList<Action*> actions();
};

inline Visualization::Item* ActionPrompt::originalActionReceiver() { return originalActionReceiver_; }
inline Visualization::Item* ActionPrompt::currentActionReceiver() { return currentActionReceiver_; }
inline Visualization::Text* ActionPrompt::text() {return actionText_;}
inline int ActionPrompt::numActions() { return actionsContainer_->length(); }
inline bool ActionPrompt::autoExecuteAction() const { return autoExecuteAction_; }

} /* namespace Interaction */
