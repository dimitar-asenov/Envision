/***********************************************************************************************************************
 * VList.cpp
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VList.h"
#include "items/Text.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VList)

VList::VList(Item* parent, Model::List* node, const VListStyle* style) :
	ModelItem(parent, node, style), items_(this, &style->itemsStyle())
{
}

VList::~VList()
{
}

void VList::determineChildren()
{
	Model::List* list = static_cast<Model::List*> (getNode());

	// TODO: find a better way and place to determine the style of children
	items_.setStyle(&style()->itemsStyle());

	items_.synchronizeWithNodes(list->nodes().toList(), renderer());
}

int VList::focusedElementIndex() const
{
	return items_.focusedElementIndex();
}

void VList::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(&items_, availableWidth, availableHeight);
}

bool VList::sizeDependsOnParent() const
{
	return items_.sizeDependsOnParent();
}

bool VList::focusChild(FocusTarget location)
{
	return items_.focusChild(location);
}

}
