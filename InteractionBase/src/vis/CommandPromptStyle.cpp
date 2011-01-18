/***********************************************************************************************************************
 * CommandPromptStyle.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/CommandPromptStyle.h"

#include "VisualizationBase/headers/Styles.h"

namespace Interaction {

CommandPromptStyle* CommandPromptStyle::getDefault()
{
	static CommandPromptStyle defaultStyle;
	return &defaultStyle;
}

void CommandPromptStyle::load()
{
	ItemStyle::load();
	Visualization::Styles::load("layout", layout_);
	Visualization::Styles::load("suggestionContainer", suggestionContainer_);
	Visualization::Styles::load("errorContainer", errorContainer_);
	Visualization::Styles::load("commandText", commandText_);
	Visualization::Styles::load("defaultSuggestion", defaultSuggestion_);
	Visualization::Styles::load("defaultError", defaultError_);
}

}
