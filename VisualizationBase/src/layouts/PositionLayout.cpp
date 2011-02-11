/***********************************************************************************************************************
 * PositionLayout.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PositionLayout.h"
#include "shapes/Shape.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( PositionLayout )

PositionLayout::PositionLayout(Item* parent, const PositionLayoutStyle* style) :
	Layout(parent, style)
{
	setAcceptedMouseButtons(Qt::LeftButton);
}

PositionLayout::~PositionLayout()
{
	for (int i = 0; i<items.size(); i++) SAFE_DELETE_ITEM( items[i]);
}

int PositionLayout::length() const
{
	return items.size();
}

bool PositionLayout::hasNode(Model::Node* node)
{
	for(int i=0; i<items.size(); ++i)
		if (items[i]->getNode() == node) return true;

	return false;
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
	items.append(mi);
	positions.append(pos);
	setUpdateNeeded();
}

void PositionLayout::remove(int index, bool deleteItem)
{
	if (deleteItem) SAFE_DELETE_ITEM( items[index]);
	items.remove(index);
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

void PositionLayout::swap(int i, int j)
{
	ModelItem* t = items[i];
	items[i] = items[j];
	items[j] = t;
}

void PositionLayout::synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer)
{
	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (i >= items.size() ) insert( renderer->render(NULL, nodes[i]));	// This node is new
		else if ( (static_cast<ModelItem*> (items[i]))->getNode() == nodes[i] )	continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k<items.size(); ++k)
			{
				if ( (static_cast<ModelItem*> (items[k]))->getNode() == nodes[i] )
				{
					// We found this node, swap the visualizations
					swap(i, k);
					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found )
			{
				insert( renderer->render(NULL, nodes[i]) );
				swap(i, items.size()-1);
			}
		}
	}

	// Remove excess items
	while (items.size() > nodes.size()) remove(items.size()-1);
}

inline int PositionLayout::toGrid(const int& pos) const
{
	int mod = (pos >=0) ? ( pos % style()->gridSize() ) : ( (-pos) % style()->gridSize() );
	int add = (pos >=0) ? ( style()->gridSize() - mod) : mod;
	return pos + ( (mod !=0) ? add : 0 );
}

bool PositionLayout::isEmpty() const
{
	for(int i = 0; i<items.size(); ++i)
		if (!items[i]->isEmpty()) return false;

	return true;
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

	int sizeWidth = bottomRight.x() - topLeft.x() + style()->leftInnerMargin() + style()->rightInnerMargin();
	int sizeHeight = bottomRight.y() - topLeft.y() + style()->topInnerMargin() + style()->bottomInnerMargin();
	setInnerSize(sizeWidth, sizeHeight);

	for (int i =0; i<items.size(); ++i)
		items[i]->setPos( xOffset() + style()->leftInnerMargin() + toGrid(positions[i]->x()) - topLeft.x(),
								yOffset() + style()->topInnerMargin() + toGrid(positions[i]->y()) - topLeft.y());
}

int PositionLayout::focusedElementIndex() const
{
	for (int i = 0; i<items.size(); ++i)
		if ( items[i]->childHasFocus()) return i;

	return -1;
}

bool PositionLayout::focusChild(FocusTarget)
{
	if (items.isEmpty()) return false;

	// TODO Implement this properly

	return Item::focusChild(items[0]);
}

}
