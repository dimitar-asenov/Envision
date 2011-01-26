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
	setFlag(QGraphicsItem::ItemHasNoContents, false);
}

VList::~VList()
{
}

void VList::determineChildren()
{
	Model::List* list = static_cast<Model::List*> (getNode());

	// TODO: find a better way and place to determine the style of children
	items_.setStyle(&style()->itemsStyle());

	// Set the children
	for (int i = 0; i < list->size(); ++i)
	{
		// Try to find an existing visualization for this node.
		int p = i;
		while (p < items_.length() && items_.at<ModelItem>(p)->getNode() != list->at<Model::Node>(i)) ++p;

		if (p < items_.length() )
		{
			// Visualization exists. Swap it if it is not here
			if (p != i) items_.swap(i,p);
		}
		else
		{
			// Visualization does not exist. Make a new one.
			items_.insert(renderer()->render(NULL, list->at<Model::Node> (i)), i);
		}
	}

	// Remove extra elements.
	while (items_.length() > list->size()) items_.remove(items_.length()-1);
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
