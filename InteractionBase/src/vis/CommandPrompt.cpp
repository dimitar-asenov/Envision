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
	command( new Text(NULL, &style->commandText())),
	result(NULL)
{
	setFlag(QGraphicsItem::ItemIsMovable);

	layout->append(command);
	layout->append(suggestionContainer);
	layout->append(errorContainer);
}

CommandPrompt::~CommandPrompt()
{
	commandReceiver_ = NULL; // This item is completely out of our control, we just know about it.

	removeResult();
	removeSuggestions();

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

void CommandPrompt::setResult(CommandResult* result_)
{
	removeResult();
	result = result_;

	// Add the errors to the list
	for (int i = 0; i<result->errors().size(); ++i)
	{
		// Create visualization if one is missing.
		if (result->errors()[i]->visualization() == NULL)
		{
			TextAndDescription* vis = new TextAndDescription(NULL, &style()->defaultError());
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
		if (result->suggestions()[i]->visualization() == NULL)
		{
			TextAndDescription* vis = new TextAndDescription(NULL, &style()->defaultSuggestion());
			vis->setContents(result->suggestions()[i]->suggestion(), result->suggestions()[i]->description());
			result->suggestions()[i]->setVisualization(vis);
		}

		// Add the visualization to the container layout
		suggestionContainer->append(result->suggestions()[i]->visualization());
	}
	setUpdateNeeded();
}

void CommandPrompt::removeResult()
{
	if (result)
	{
		// Remove all suggestion visual items contributed by the result.
		for (int i = 0; i<result->suggestions().size(); ++i)
			suggestionContainer->removeAll(result->suggestions().at(i)->visualization(), false);

		// Remove all error visual items contributed by the result.
		for (int i = 0; i<result->errors().size(); ++i)
			errorContainer->removeAll(result->errors().at(i)->visualization(), false);
	}
	SAFE_DELETE(result);
	setUpdateNeeded();
}

void CommandPrompt::addSuggestion(CommandSuggestion* suggestion)
{
	suggestions.append(suggestion);

	// Create visualization if one is missing.
	if (suggestion->visualization() == NULL)
	{
		TextAndDescription* vis = new TextAndDescription(NULL, &style()->defaultSuggestion());
		vis->setContents(suggestion->suggestion(), suggestion->description());
		suggestion->setVisualization(vis);
	}

	// Add the visualization to the container layout
	suggestionContainer->append(suggestion->visualization());

	setUpdateNeeded();
}

void CommandPrompt::addSuggestions(const QList<CommandSuggestion*>& suggestions_)
{
	for (int i = 0; i < suggestions_.size(); ++i)
	{
		suggestions.append(suggestions_[i]);

		// Create visualization if one is missing.
		if (suggestions.last()->visualization() == NULL)
		{
			TextAndDescription* vis = new TextAndDescription(NULL, &style()->defaultSuggestion());
			vis->setContents(suggestions.last()->suggestion(), suggestions.last()->description());
			suggestions.last()->setVisualization(vis);
		}

		// Add the visualization to the container layout
		suggestionContainer->append(suggestions.last()->visualization());
	}
	setUpdateNeeded();
}

void CommandPrompt::removeSuggestions()
{
	for (int i = 0; i<suggestions.size(); ++i)
	{
		suggestionContainer->removeAll(suggestions.at(i)->visualization(), false);
		SAFE_DELETE(suggestions[i]);
	}
	suggestions.clear();
	setUpdateNeeded();
}

}
