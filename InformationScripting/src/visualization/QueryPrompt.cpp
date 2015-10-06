/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "QueryPrompt.h"

#include "VisualizationBase/src/cursor/TextCursor.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

#include "InteractionBase/src/handlers/GenericHandler.h"

#include "../nodes/QueryNodeContainer.h"
#include "../nodes/EmptyQueryNode.h"

namespace InformationScripting {

/**
 * Specifies the minimal height of the commandReceiver item which will make a new prompt appear in the center of the
 * item.
 *
 * If the receiver item's height is less than this number, then the prompt will be placed directly under the item.
 * Otherwise the prompt will be centered inside the item.
 */
const int COMMAND_RECEIVER_ITEM_MIN_PROMPT_CENTER_HEIGHT = 50;

/**
 * The distance between a prompt which is shown under the command receiver item and that item.
 */
const int PROMPT_TO_RECEIVER_DISTANCE = 3;

ITEM_COMMON_DEFINITIONS(QueryPrompt, "item")


QueryPrompt::QueryPrompt(Visualization::Item* commandReceiver, QString, bool centerOnPrompt,
								 const StyleType* style) :
	Super(nullptr, style),
	commandReceiver_(commandReceiver)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(ItemIgnoresTransformations);
	setZValue(LAYER_COMMAND);
	setItemCategory(Visualization::Scene::MenuItemCategory);

	commandReceiver->scene()->addTopLevelItem(this);

	saveReceiverCursor();
	setPromptPosition();

	if (centerOnPrompt) centerViewOnPrompt();

	queryContainer_ = new QueryNodeContainer();
	queryContainer_->setQuery(new EmptyQueryNode);
}

QueryPrompt::~QueryPrompt()
{
	if (scene()) scene()->removeTopLevelItem(this);
	commandReceiver_ = nullptr; // This item is completely out of our control, we just know about it.

	// These are deleted by layout's destructor
	// TODO this is copied from CommandPrompt is it correct here as well?
	queryContainer_ = nullptr;
	errorText_ = nullptr;
}

void QueryPrompt::initializeForms()
{
	auto queryNodeEl = item(&I::queryVis_, [](I* v){return v->queryContainer_;});

	auto errorEl = item(&I::errorText_);

	addForm(grid({
			{queryNodeEl},
			{errorEl}
		})
		->setNoInnerCursors([](Item*){return true;})
		->setNoBoundaryCursors([](Item*){return true;})
	);
}

void QueryPrompt::executeCurrentText()
{
//	commandReceiver_->execute( command_->text(), commandReceiverCursor_);
}

void QueryPrompt::showPrompt(QString, bool centerOnPrompt)
{
	hideRequested_ = false;
	saveReceiverCursor();
	setPromptPosition();
	show();

	if (centerOnPrompt) centerViewOnPrompt();

	wasCancelled_ = false;
}

void QueryPrompt::hidePrompt()
{
	hideRequested_ = true;

	hide();

	if (scene()->mainCursor()) // If the main cursor was deleted, then do not select anything.
		commandReceiver()->moveCursor(Visualization::Item::MoveOnPosition, receiverCursorPosition());
}

void QueryPrompt::saveReceiverCursor()
{
	commandReceiverCursor_.reset();
	if (commandReceiver_->scene()->mainCursor() && commandReceiver_->scene()->mainCursor()->owner() == commandReceiver_)
		commandReceiverCursor_.reset(commandReceiver_->scene()->mainCursor()->clone());
}

void QueryPrompt::setPromptPosition()
{
	QPointF promptPos = commandReceiver_->mapToScene(0, 0);
	if (commandReceiver_->heightInScene() < COMMAND_RECEIVER_ITEM_MIN_PROMPT_CENTER_HEIGHT)
	{
		// Show the prompt under the receiver item.
		promptPos.setY( promptPos.y() + commandReceiver_->heightInScene() + PROMPT_TO_RECEIVER_DISTANCE);
	}
	else
	{
		// If the item is rather large show the prompt at the cursor
		promptPos += receiverCursorPosition();
	}

	setPos(promptPos);
}

QPoint QueryPrompt::receiverCursorPosition()
{
	return commandReceiverCursor_ ? commandReceiverCursor_->position() : QPoint(0, 0);
}

void QueryPrompt::centerViewOnPrompt() const
{
	for (auto v : commandReceiver_->scene()->views())
		if (auto mainView = dynamic_cast<Visualization::MainView*>(v))
		{
			mainView->centerOn(pos());
			break;
		}
}

} /* namespace InformationScripting */
