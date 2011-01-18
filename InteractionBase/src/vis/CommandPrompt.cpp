/***********************************************************************************************************************
 * CommandPrompt.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/CommandPrompt.h"
#include "vis/TextAndDescription.h"

using namespace Visualization;

namespace Interaction {

ITEM_COMMON_DEFINITIONS(CommandPrompt)

CommandPrompt::CommandPrompt(Item* commandReceiver, const CommandPromptStyle* style) :
	Item(NULL, style),
	commandReceiver_(commandReceiver),
	layout(new SequentialLayout(this, &style->layout())),
	suggestionContainer(new SequentialLayout(NULL, &style->suggestionContainer())),
	errorContainer(new SequentialLayout(NULL, &style->errorContainer())),
	command( new Text(NULL, &style->commandText()))
{
	setFlag(QGraphicsItem::ItemIsMovable);

	layout->append(command);
	layout->append(suggestionContainer);
	layout->append(errorContainer);
}

CommandPrompt::~CommandPrompt()
{
	commandReceiver_ = NULL; // This item is completely out of our control, we just know about it.

	deleteMessages(suggestions, suggestionContainer);
	deleteMessages(errors, errorContainer);

	SAFE_DELETE_ITEM(layout);

	// These are deleted by layout's destructor
	command = NULL;
	suggestionContainer = NULL;
	errorContainer = NULL;
}

void CommandPrompt::initializeCommand()
{
	command->setText("Type a command");
	command->setFocus();
	command->setSelected(1000, 0);
}

void CommandPrompt::determineChildren()
{
}

void CommandPrompt::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout, availableWidth, availableHeight);
}

template <class T>
void CommandPrompt::deleteMessages(QList<T*>& messages, Visualization::SequentialLayout*& containerLayout)
{
	for (int i = 0; i<messages.size(); ++i) SAFE_DELETE(messages[i]);
	messages.clear();
	containerLayout->removeAll(true);
}

void CommandPrompt::setSuggestions(const QList<CommandSuggestion*>& suggestions_)
{
	deleteMessages(suggestions, suggestionContainer);
	suggestions = suggestions_;

	// Create visualization if one is missing.
	for (int i = 0; i<suggestions.size(); ++i)
	{
		if (suggestions[i]->visualization() == NULL)
		{
			TextAndDescription* vis = new TextAndDescription(NULL, &style()->defaultSuggestion());
			vis->setContents(suggestions[i]->suggestion(), suggestions[i]->description());
			suggestions[i]->setVisualization(vis);
		}

		// Add the visualization to the container layout
		suggestionContainer->append(suggestions[i]->visualization());
	}
}

void CommandPrompt::setErrors(const QList<CommandError*>& errors_)
{
	deleteMessages(errors, errorContainer);
	errors = errors_;

	// Create visualization if one is missing.
	for (int i = 0; i<errors.size(); ++i)
	{
		if (errors[i]->visualization() == NULL)
		{
			TextAndDescription* vis = new TextAndDescription(NULL, &style()->defaultError());
			vis->setContents(errors[i]->message(), errors[i]->resolutionTips().join(" OR "));
			errors[i]->setVisualization(vis);
		}

		// Add the visualization to the container layout
		errorContainer->append(errors[i]->visualization());
	}
}

}
