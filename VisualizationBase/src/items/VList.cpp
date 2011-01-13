/***********************************************************************************************************************
 * VList.cpp
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VList.h"
#include "items/Text.h"

#include <QtCore/QDebug>

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VList)

VList::VList(Item* parent, Model::List* node, const VListStyle* style) :
	ModelItem(parent, node, style), items_(this, &style->itemsStyle())
{
	// TODO remove this from here, it was used just for testing
	setFlag(QGraphicsItem::ItemIsMovable);
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
	// TODO this can be done smarter

	bool changed = false;
	for (int i = 0; i < list->size(); ++i)
	{
		if ( !changed )
		{
			changed = items_.length() <= i;
			if ( !changed ) changed = items_.at<ModelItem> (i)->getNode() != list->at<Model::Node> (i);

			if ( changed ) while ( items_.length() > i )
				items_.remove(items_.length() - 1);
		}

		if ( changed ) items_.append(renderer()->render(NULL, list->at<Model::Node> (i)));
	}

}

void VList::updateState()
{
	bounding_rect = items_.boundingRect();
	size.setWidth(items_.width());
	size.setHeight(items_.height());
}

void VList::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
	// Currently, a shape on the list itself is not supported. Put a shape on the layout instead.
	//if ( getShape() ) ModelItem::paint(painter, option, widget);
}

}
