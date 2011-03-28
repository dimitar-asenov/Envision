/***********************************************************************************************************************
 * Text.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/Text.h"

namespace Visualization {
ITEM_COMMON_DEFINITIONS(Text, "item")

Text::Text(Item* parent, const QString& text) :
	TextRenderer(parent, itemStyles().get("darkGrey"), text)
{
	setEditable(false);
}

Text::Text(Item* parent, const StyleType *style, const QString& text) :
	TextRenderer(parent, style, text)
{
	setEditable(false);
}


QString Text::currentText()
{
	return text();
}

}
