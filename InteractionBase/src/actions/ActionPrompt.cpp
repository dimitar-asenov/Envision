/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * ActionPrompt.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Dimitar Asenov
 */

#include "ActionPrompt.h"
#include "../vis/TextAndDescription.h"
#include "Action.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/cursor/Cursor.h"
//#include "VisualizationBase/src/cursor/TextCursor.h"
//#include "VisualizationBase/src/items/SceneHandlerItem.h"

using namespace Visualization;

namespace Interaction {

ITEM_COMMON_DEFINITIONS(ActionPrompt, "item")

ActionPrompt::ActionPrompt(Item* actionReceiver,  bool autoExecuteAction, const StyleType* style) :
	Item(nullptr, style),
	autoExecuteAction_(autoExecuteAction),
	actionReceiver_(actionReceiver),
	layout_(new SequentialLayout(this, &style->layout())),
	actionsContainer_(new SequentialLayout(layout_, &style->actionsContainer())),
	actionText_( new Text(layout_, &style->shortcutText()))
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(ItemIgnoresTransformations);
	setZValue(LAYER_COMMAND);
	setItemCategory(Scene::MenuItemCategory);

	layout_->append(actionText_);
	layout_->append(actionsContainer_);

	actionReceiver_->scene()->addTopLevelItem(this);

	actionText_->setText("");
	setPromptPosition();
	acquireCursor();
}

ActionPrompt::~ActionPrompt()
{
	if(scene()) scene()->removeTopLevelItem(this);
	actionReceiver_ = nullptr; // This item is completely out of our control, we just know about it.

	SAFE_DELETE_ITEM(layout_);

	// These are deleted by layout's destructor
	actionText_ = nullptr;
	actionsContainer_ = nullptr;
}

void ActionPrompt::showPrompt()
{
	actionText_->setText("");
	setPromptPosition();
	show();
	acquireCursor();
}

void ActionPrompt::hidePrompt()
{
	hide();
	actionReceiver_->moveCursor(Visualization::Item::MoveOnPosition, receiverCursorPosition_);
}

void ActionPrompt::determineChildren()
{
	actionsContainer_->clear();

	for(auto a : Action::actions(actionReceiver_->node()))
	{
		if (a->shortcut().startsWith(actionText_->text()))
			actionsContainer_->append(new TextAndDescription(a->shortcut(), a->name(), &style()->actionStyle()));
	}
}

void ActionPrompt::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout_, availableWidth, availableHeight);
}

void ActionPrompt::acquireCursor()
{
	// Save the current cursor
	receiverCursorPosition_ = QPoint(0,0);
	if (actionReceiver_->scene()->mainCursor()->owner() == actionReceiver_)
		receiverCursorPosition_ = actionReceiver_->scene()->mainCursor()->position();

	actionText_->moveCursor();
}

void ActionPrompt::setPromptPosition()
{
	for( auto v : actionReceiver_->scene()->views())
	{
		if (v->isActiveWindow())
		{
			setPos(v->mapToScene(210, v->height() - 40)); // Put it after the mini map on the bottom
			break;
		}
	}
}

} /* namespace Interaction */
