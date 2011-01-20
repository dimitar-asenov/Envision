/***********************************************************************************************************************
 * SelectedItem.cpp
 *
 *  Created on: Jan 20, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SelectedItem.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(SelectedItem)

SelectedItem::SelectedItem(Item* selectedItem, const SelectedItemStyle* style) :
	Item(NULL, style), selectedItem_(selectedItem)
{
	setFlags(0);
	setAcceptedMouseButtons(0);
	setZValue(LAYER_SELECTION_Z);
}

SelectedItem::~SelectedItem()
{
	selectedItem_ = NULL;
}

bool SelectedItem::needsUpdate()
{
	return true;
}

void SelectedItem::determineChildren()
{

}

void SelectedItem::updateGeometry(int, int)
{
	if (style()->hasShape())
	{
		getShape()->setInnerSize(selectedItem_->width(), selectedItem_->height());
		QPointF pos = QPointF( getShape()->contentLeft(), getShape()->contentTop() );

		setPos(selectedItem_->mapToScene(0,0) - pos);
	}
}

}
