/***********************************************************************************************************************
 * CommandSuggestion.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/CommandSuggestion.h"

#include "VisualizationBase/headers/items/Item.h"

namespace Interaction {

CommandSuggestion::CommandSuggestion(const QString& suggestion, const QString& description, Visualization::Item* visualization) :
	suggestedCommand(suggestion), commandDescription(description), suggestionVisualization(visualization)
{
}

CommandSuggestion::~CommandSuggestion()
{
	// do not delete this, this is the reponsibility of the object handling this suggestion.
	suggestionVisualization = NULL;
}

}
