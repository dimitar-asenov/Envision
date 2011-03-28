/***********************************************************************************************************************
 * TextAndDescriptionStyle.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/TextAndDescriptionStyle.h"

namespace Interaction {

void TextAndDescriptionStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("layout", layout_);
	sl.load("text", text_);
	sl.load("description", description_);
}

}
