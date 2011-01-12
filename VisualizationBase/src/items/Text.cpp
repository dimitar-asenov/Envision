/***********************************************************************************************************************
 * Text.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/Text.h"

#include "Styles.h"

namespace Visualization {
ITEM_COMMON_DEFINITIONS(Text)

Text::Text(Item* parent, const QString& text) :
	TextRenderer<Item>(parent, Styles::item<Text>("default"), text)
{
}

Text::Text(Item* parent, const TextStyle *style, const QString& text) :
	TextRenderer<Item>(parent, style, text)
{
}

}
