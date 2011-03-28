/***********************************************************************************************************************
 * CommandPromptStyle.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/CommandPromptStyle.h"

namespace Interaction {

void CommandPromptStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("layout", layout_);
	sl.load("suggestionContainer", suggestionContainer_);
	sl.load("errorContainer", errorContainer_);
	sl.load("commandText", commandText_);
	sl.load("defaultSuggestion", defaultSuggestion_);
	sl.load("defaultError", defaultError_);
}

}
