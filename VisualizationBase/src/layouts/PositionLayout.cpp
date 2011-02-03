/***********************************************************************************************************************
 * PositionLayout.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PositionLayout.h"
#include "shapes/Shape.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( PositionLayout )

PositionLayout::PositionLayout(Item* parent, const PositionLayoutStyle* style) :
	Layout(parent, style)
{
}

PositionLayout::~PositionLayout()
{
	for (int i = 0; i<items.size(); i++) SAFE_DELETE_ITEM( items[i]);
}

int PositionLayout::length() const
{
	return items.size();
}

void PositionLayout::insert(Item* item)
{
	// Check whether this item has a position associated with it
	ModelItem* mi = dynamic_cast<ModelItem*> (item);
	if (!mi) throw VisualizationException("Adding an Item that is not a ModelItem to a PositionLayout");

	Model::ExtendableNode* node = dynamic_cast<Model::ExtendableNode*> (mi->getNode());
	if (!node) throw VisualizationException("Adding a ModelItem that does not implement ExtendableNode to a PositionLayout");

	Position* pos = node->extension<Position>();

	if (!pos) throw VisualizationException("Adding a ModelItem whose node does not have a Position extension to a PositionLayout");

	item->setParentItem(this);
	items.append(item);
	positions.append(pos);
	setUpdateNeeded();
}

void PositionLayout::removeAll(Item* item, bool deleteItem)
{
	if (deleteItem) SAFE_DELETE_ITEM(item);
	for (int i = items.size() - 1; i>=0; --i)
		if (items.at(i) == item) items.remove(i);
	setUpdateNeeded();
}

void PositionLayout::clear(bool deleteItems)
{
	if (deleteItems) for (int i = 0; i<items.size(); ++i) SAFE_DELETE_ITEM(items[i]);
	items.clear();
}

inline int PositionLayout::toGrid(const int& pos) const
{
	return pos + (pos >=0) ? (-(pos % style()->gridSize())) : ( (-pos) % style()->gridSize());
}

void PositionLayout::updateGeometry(int, int)
{
	QPoint topLeft;
	QPoint bottomRight;

	for(int i = 0; i<items.size(); ++i)
	{
		if (i==0 || topLeft.x() > toGrid(positions[i]->x()) ) topLeft.setX( toGrid(positions[i]->x()) );
		if (i==0 || topLeft.y() > toGrid(positions[i]->y()) ) topLeft.setY( toGrid(positions[i]->y()) );
		if (i==0 || bottomRight.x() < toGrid(positions[i]->x()) + items[i]->width()  ) bottomRight.setX( toGrid(positions[i]->x()) + items[i]->width() );
		if (i==0 || bottomRight.y() < toGrid(positions[i]->y()) + items[i]->height() ) bottomRight.setY( toGrid(positions[i]->y()) + items[i]->height() );
	}

	int sizeWidth = bottomRight.x() - topLeft.x() + style()->leftMargin() + style()->rightMargin();
	int sizeHeight = bottomRight.y() - topLeft.y() + style()->topMargin() + style()->bottomMargin();
	setInnerSize(sizeWidth, sizeHeight);

	for (int i =0; i<items.size(); ++i) items[i]->setPos( xOffset() + toGrid(positions[i]->x()) - topLeft.x(), yOffset() +  toGrid(positions[i]->y()) - topLeft.y());
}

int PositionLayout::focusedElementIndex() const
{
	for (int i = 0; i<items.size(); ++i)
		if ( items[i]->childHasFocus()) return i;

	return -1;
}

bool PositionLayout::focusChild(FocusTarget location)
{
	if (items.isEmpty()) return false;

	// TODO Implement this properly

	return Item::focusChild(items[0]);
}

}
