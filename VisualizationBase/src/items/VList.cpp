/***********************************************************************************************************************
 * VList.cpp
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VList.h"
#include "items/Text.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VList, "item")

VList::VList(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<>, Model::List>(parent, node, style)
{
}

void VList::determineChildren()
{
	// TODO: find a better way and place to determine the style of children
	layout()->setStyle(&style()->itemsStyle());
	layout()->synchronizeWithNodes(node()->nodes().toList(), renderer());
}

}
