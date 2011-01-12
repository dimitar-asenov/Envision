/***********************************************************************************************************************
 * VText.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VText.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VText)

VText::VText(Item* parent, Model::Text *text, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, text, style)
{
}

void VText::updateState()
{
	Model::Text* textNode = static_cast<Model::Text*> (getNode());
	setText( textNode->get() );
	TextRenderer<ModelItem>::updateState();
}

}
