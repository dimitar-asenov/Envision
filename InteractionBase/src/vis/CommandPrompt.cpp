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

#include "vis/CommandPrompt.h"
#include "vis/TextAndDescription.h"
#include "handlers/GenericHandler.h"
#include "commands/CommandExecutionEngine.h"
#include "autocomplete/AutoComplete.h"
#include "autocomplete/AutoCompleteEntry.h"

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

CommandPrompt::CommandPrompt(Item* commandReceiver, QString initialCommandText, const StyleType* style) :
	Super(nullptr, style),
	commandReceiver_(commandReceiver),
	layout_(new SequentialLayout(this, &style->layout())),
	errorContainer_(new SequentialLayout(layout_, &style->errorContainer())),
	command_( new Text(layout_, &style->commandText()))
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(ItemIgnoresTransformations);
	setZValue(LAYER_COMMAND);
	setItemCategory(Scene::MenuItemCategory);

	command_->setEditable(true);

	layout_->append(command_);
	layout_->append(errorContainer_);

	commandReceiver->scene()->addTopLevelItem(this);

	if (initialCommandText.isNull()) command_->setText("Type a command");
	else command_->setText(initialCommandText);
	saveReceiverCursor();
	setPromptPosition();
	command_->moveCursor();

	if (initialCommandText.isNull())
		command_->correspondingSceneCursor<Visualization::TextCursor>()->selectAll();
	else
		command_->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(initialCommandText.size());
}

CommandPrompt::~CommandPrompt()
{
	if (scene()) scene()->removeTopLevelItem(this);
	commandReceiver_ = nullptr; // This item is completely out of our control, we just know about it.

	removeResult();
	removeSuggestions();

	SAFE_DELETE_ITEM(layout_);

	// These are deleted by layout's destructor
	command_ = nullptr;
	errorContainer_ = nullptr;
}

inline QPoint CommandPrompt::receiverCursorPosition()
{
	return commandReceiverCursor_ ? commandReceiverCursor_->position() : QPoint(0, 0);
}

void CommandPrompt::takeSuggestion(CommandSuggestion* suggestion)
{
	command_->setText(suggestion->text());
	command_->moveCursor();
	command_->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(suggestion->text().size());
}

void CommandPrompt::showPrompt(QString initialCommandText)
{
	saveReceiverCursor();
	setPromptPosition();
	show();
	if (! initialCommandText.isNull()) command_->setText(initialCommandText);
	command_->moveCursor();
	command_->correspondingSceneCursor<Visualization::TextCursor>()
			->setSelectedCharacters(commandSelectedFirst_, commandSelectedLast_);
}

void CommandPrompt::hidePrompt()
{
	if (scene()->mainCursor())
	{
		commandSelectedFirst_ = command_->correspondingSceneCursor<Visualization::TextCursor>()->selectionFirstIndex();
		commandSelectedLast_ = command_->correspondingSceneCursor<Visualization::TextCursor>()->selectionLastIndex();
	}
	hide();

	if (scene()->mainCursor()) // If the main cursor was deleted, then do not select anything.
		commandReceiver()->moveCursor(Visualization::Item::MoveOnPosition, receiverCursorPosition());
}

void CommandPrompt::determineChildren()
{
	GenericHandler* h = dynamic_cast<GenericHandler*> (handler());

	if (h)
	{
		removeSuggestions();
		addSuggestions( h->executionEngine()->autoComplete(commandReceiver_, command_->text(), commandReceiverCursor_));
	}
}

void CommandPrompt::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout_, availableWidth, availableHeight);
}

void CommandPrompt::setResult(QSharedPointer<CommandResult> result)
{
	removeResult();
	result_ = result;

	// Add the errors to the list
	for (int i = 0; i<result->errors().size(); ++i)
	{
		// Create visualization if one is missing.
		if (result->errors()[i]->visualization() == nullptr)
		{
			TextAndDescription* vis = new TextAndDescription(errorContainer_, &style()->defaultError());
			vis->setContents(result->errors()[i]->message(), result->errors()[i]->resolutionTips().join(" OR "));
			result->errors()[i]->setVisualization(vis);
		}

		// Add the visualization to the container layout
		errorContainer_->append(result->errors()[i]->visualization());
	}

	// Add the suggestions to the list
	addSuggestions(result->suggestions());
	showAutocompleteBasedOnSuggestions();
}

void CommandPrompt::removeResult()
{
	if (result_)
	{
		// Remove all suggestion contributed by the result.
		for (auto s : result_->suggestions()) suggestions_.removeAll(s);

		// Remove all error visual items contributed by the result.
		errorContainer_->clear(false);
	}
	result_.clear();

	showAutocompleteBasedOnSuggestions();
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::addSuggestion(CommandSuggestion* suggestion)
{
	suggestions_.append(suggestion);
	showAutocompleteBasedOnSuggestions();
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::addSuggestions(const QList<CommandSuggestion*>& suggestions)
{
	suggestions_.append( suggestions );
	showAutocompleteBasedOnSuggestions();
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::removeSuggestions()
{
	for (int i = 0; i<suggestions_.size(); ++i) SAFE_DELETE(suggestions_[i]);
	AutoComplete::hide();
	suggestions_.clear();
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void CommandPrompt::saveReceiverCursor()
{
	commandReceiverCursor_.reset();
	if (commandReceiver_->scene()->mainCursor() && commandReceiver_->scene()->mainCursor()->owner() == commandReceiver_)
		commandReceiverCursor_.reset(commandReceiver_->scene()->mainCursor()->clone());
}

void CommandPrompt::setPromptPosition()
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

void CommandPrompt::showAutocompleteBasedOnSuggestions()
{
	auto executeFunction = [](AutoCompleteEntry* e){
		if (auto prompt = GenericHandler::instance()->commandPrompt())
			prompt->takeSuggestion(static_cast<CommandSuggestion*>(e));
	};


	QList<AutoCompleteEntry*> entries;
		for (auto s : suggestions_) entries.append(new AutoCompleteEntry(s->text(), s->description(),
				s->visualization(), executeFunction));

	if (entries.isEmpty() || !isVisible())
		AutoComplete::hide();
	else
		AutoComplete::show(entries, true);
}

}
