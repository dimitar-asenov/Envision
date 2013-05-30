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

#include "vis/CommandPrompt.h"
#include "vis/TextAndDescription.h"
#include "handlers/GenericHandler.h"
#include "commands/CommandExecutionEngine.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/cursor/TextCursor.h"
#include "VisualizationBase/src/items/SceneHandlerItem.h"

using namespace Visualization;

namespace Interaction {

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

ITEM_COMMON_DEFINITIONS(CommandPrompt, "item")

CommandPrompt::CommandPrompt(Item* commandReceiver, const StyleType* style) :
	Super(nullptr, style),
	commandReceiver_(commandReceiver),
	layout(new SequentialLayout(this, &style->layout())),
	suggestionContainer(new SequentialLayout(layout, &style->suggestionContainer())),
	errorContainer(new SequentialLayout(layout, &style->errorContainer())),
	command( new Text(layout, &style->commandText())),
	result_(nullptr)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(ItemIgnoresTransformations);
	setZValue(LAYER_COMMAND);
	setItemCategory(Scene::MenuItemCategory);

	command->setEditable(true);

	layout->append(command);
	layout->append(errorContainer);
	layout->append(suggestionContainer);

	commandReceiver->scene()->addTopLevelItem(this);

	command->setText("Type a command");
	saveReceiverCursorPosition();
	setPromptPosition();
	command->moveCursor();
	command->correspondingSceneCursor<Visualization::TextCursor>()->selectAll();
}

CommandPrompt::~CommandPrompt()
{
	if(scene()) scene()->removeTopLevelItem(this);
	commandReceiver_ = nullptr; // This item is completely out of our control, we just know about it.

	removeResult();
	removeSuggestions();

	SAFE_DELETE_ITEM(layout);

	// These are deleted by layout's destructor
	command = nullptr;
	suggestionContainer = nullptr;
	errorContainer = nullptr;
}

void CommandPrompt::takeSuggestion(CommandSuggestion* suggestion)
{
	command->setText(suggestion->suggestion());
	command->moveCursor();
	command->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(suggestion->suggestion().size());
}

void CommandPrompt::showPrompt()
{
	saveReceiverCursorPosition();
	setPromptPosition();
	show();
	command->moveCursor();
	command->correspondingSceneCursor<Visualization::TextCursor>()
			->setSelectedCharacters(commandSelectedFirst, commandSelectedLast);
}

void CommandPrompt::hidePrompt()
{
	commandSelectedFirst = command->correspondingSceneCursor<Visualization::TextCursor>()->selectionFirstIndex();
	commandSelectedLast = command->correspondingSceneCursor<Visualization::TextCursor>()->selectionLastIndex();
	hide();
	commandReceiver()->moveCursor(Visualization::Item::MoveOnPosition, receiverCursorPosition);
}

void CommandPrompt::determineChildren()
{
	GenericHandler* h = dynamic_cast<GenericHandler*> (handler());

	if (h)
	{
		removeSuggestions();
		addSuggestions( h->executionEngine()->autoComplete(commandReceiver_, command->text()));
	}
}

void CommandPrompt::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout, availableWidth, availableHeight);
}

void CommandPrompt::setResult(CommandResult* result)
{
	removeResult();
	result_ = result;

	// Add the errors to the list
	for (int i = 0; i<result->errors().size(); ++i)
	{
		// Create visualization if one is missing.
		if (result->errors()[i]->visualization() == nullptr)
		{
			TextAndDescription* vis = new TextAndDescription(errorContainer, &style()->defaultError());
			vis->setContents(result->errors()[i]->message(), result->errors()[i]->resolutionTips().join(" OR "));
			result->errors()[i]->setVisualization(vis);
		}

		// Add the visualization to the container layout
		errorContainer->append(result->errors()[i]->visualization());
	}

	// Add the suggestions to the list
	for (int i = 0; i<result->suggestions().size(); ++i)
	{
		// Create visualization if one is missing.
		if (result->suggestions()[i]->visualization() == nullptr)
		{
			TextAndDescription* vis = new TextAndDescription(suggestionContainer, &style()->defaultSuggestion());
			vis->setContents(result->suggestions()[i]->suggestion(), result->suggestions()[i]->description());
			result->suggestions()[i]->setVisualization(vis);
		}

		// Add the visualization to the container layout
		suggestionContainer->append(result->suggestions()[i]->visualization());
	}
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::removeResult()
{
	if (result_)
	{
		// Remove all suggestion visual items contributed by the result.
		for (int i = 0; i<result_->suggestions().size(); ++i)
			suggestionContainer->removeAll(result_->suggestions().at(i)->visualization(), false);

		// Remove all error visual items contributed by the result.
		for (int i = 0; i<result_->errors().size(); ++i)
			errorContainer->removeAll(result_->errors().at(i)->visualization(), false);
	}
	SAFE_DELETE(result_);
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::addSuggestion(CommandSuggestion* suggestion)
{
	suggestions_.append(suggestion);

	// Create visualization if one is missing.
	if (suggestion->visualization() == nullptr)
	{
		TextAndDescription* vis = new TextAndDescription(suggestionContainer, &style()->defaultSuggestion());
		vis->setContents(suggestion->suggestion(), suggestion->description());
		suggestion->setVisualization(vis);
	}

	// Add the visualization to the container layout
	suggestionContainer->append(suggestion->visualization());

	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::addSuggestions(const QList<CommandSuggestion*>& suggestions)
{
	for (int i = 0; i < suggestions.size(); ++i)
	{
		suggestions_.append(suggestions[i]);

		// Create visualization if one is missing.
		if (suggestions_.last()->visualization() == nullptr)
		{
			TextAndDescription* vis = new TextAndDescription(suggestionContainer, &style()->defaultSuggestion());
			vis->setContents(suggestions_.last()->suggestion(), suggestions_.last()->description());
			suggestions_.last()->setVisualization(vis);
		}

		// Add the visualization to the container layout
		suggestionContainer->append(suggestions_.last()->visualization());
	}
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::removeSuggestions()
{
	for (int i = 0; i<suggestions_.size(); ++i)
	{
		suggestionContainer->removeAll(suggestions_.at(i)->visualization(), false);
		SAFE_DELETE(suggestions_[i]);
	}
	suggestions_.clear();
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::saveReceiverCursorPosition()
{
	// Save the current cursor
	receiverCursorPosition = QPoint(0,0);
	if (commandReceiver_->scene()->mainCursor()->owner() == commandReceiver_)
		receiverCursorPosition = commandReceiver_->scene()->mainCursor()->position();
}

void CommandPrompt::setPromptPosition()
{
	QPointF promptPos = commandReceiver_->mapToScene(0,0);
	if (commandReceiver_->height() < COMMAND_RECEIVER_ITEM_MIN_PROMPT_CENTER_HEIGHT)
	{
		// Show the prompt under the receiver item.
		promptPos.setY( promptPos.y() + commandReceiver_->height() + PROMPT_TO_RECEIVER_DISTANCE);
	}
	else
	{
		// If the item is rather large show the prompt at the cursor
		promptPos += receiverCursorPosition;
	}

	setPos(promptPos);
}

}
