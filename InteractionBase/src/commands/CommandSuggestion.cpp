/***********************************************************************************************************************
 * CommandSuggestion.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/CommandSuggestion.h"

namespace Interaction {

CommandSuggestion::CommandSuggestion(const QString& suggestion, const QString& description, Visualization::Item* visualization) :
	suggestedCommand(suggestion), commandDescription(description), suggestionVisualization(visualization)
{
}

CommandSuggestion::~CommandSuggestion()
{
}

}
