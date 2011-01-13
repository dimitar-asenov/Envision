/***********************************************************************************************************************
 * PanelLayout.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelLayout.h"
#include "items/NoItem.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( PanelLayout )

PanelLayout::PanelLayout(Item* parent, const PanelLayoutStyle* style) :
	Layout(parent, style), first_(new NoItem(this)), middle_(new NoItem(this)), last_(new NoItem(this)), minimalLength(0)
{
}

PanelLayout::~PanelLayout()
{
	SAFE_DELETE_ITEM(first_);
	SAFE_DELETE_ITEM(middle_);
	SAFE_DELETE_ITEM(last_);
}
void PanelLayout::setItem(Item* item, Item*& position, bool deleteOldItem)
{
	if (deleteOldItem) SAFE_DELETE_ITEM( position );
	if ( !item ) item = new NoItem(NULL);
	item->setParentItem(this);
	position = item;
	setUpdateNeeded();
}

void PanelLayout::updateState()
{
	if ( style()->orientation() == PanelLayoutStyle::HorizontalOrientation )
	{
		// Get the width of the area where we should put the sequential components
		int width = first_->width() + middle_->width() + last_->width() + 2 * style()->spaceBetweenElements();

		int outterWidth = width + style()->leftMargin() + style()->rightMargin();
		if ( getShape() ) outterWidth = getShape()->getOutterWidth(outterWidth);

		if ( minimalLength > outterWidth ) width += minimalLength - outterWidth;

		// Get the height
		int maxChildHeight = first_->height();
		if ( middle_->height() > maxChildHeight ) maxChildHeight = middle_->height();
		if ( last_->height() > maxChildHeight ) maxChildHeight = last_->height();

		// Set the size
		setInnerSize(width, maxChildHeight);

		// Set the position of all items
		int y = yOffset();

		// Begin
		int x = xOffset();
		if ( style()->alignment() == PanelLayoutStyle::BottomAlignment ) first_->setPos(x, y + maxChildHeight - first_->height());
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) first_->setPos(x, y + (maxChildHeight - first_->height()) / 2);

		// End
		x = xOffset() + width - last_->width();
		if ( style()->alignment() == PanelLayoutStyle::BottomAlignment ) last_->setPos(x, y + maxChildHeight - last_->height());
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) last_->setPos(x, y + (maxChildHeight - last_->height()) / 2);

		// Center
		x = xOffset() + (width - middle_->width()) / 2;
		int minX = first_->pos().x() + first_->width() + style()->spaceBetweenElements();
		if ( x < minX ) x = minX;
		if ( style()->alignment() == PanelLayoutStyle::BottomAlignment ) middle_->setPos(x, y + maxChildHeight - middle_->height());
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) middle_->setPos(x, y + (maxChildHeight - middle_->height()) / 2);
	}
	else
	{
		// Get the height of the area where we should put the sequential components
		int height = first_->height() + middle_->height() + last_->height() + 2 * style()->spaceBetweenElements();

		int outterHeight = height + style()->topMargin() + style()->bottomMargin();
		if ( getShape() ) outterHeight = getShape()->getOutterHeight(outterHeight);

		if ( minimalLength > outterHeight ) height += minimalLength - outterHeight;

		// Get the width
		int maxChildWidth = first_->width();
		if ( middle_->width() > maxChildWidth ) maxChildWidth = middle_->width();
		if ( last_->width() > maxChildWidth ) maxChildWidth = last_->width();

		// Set the size
		setInnerSize(maxChildWidth, height);

		// Set the position of all items
		int x = xOffset();

		// Begin
		int y = yOffset();
		if ( style()->alignment() == PanelLayoutStyle::RightAlignment ) first_->setPos(x + maxChildWidth - first_->width(), y);
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) first_->setPos(x + (maxChildWidth - first_->width()) / 2, y);

		// End
		y = yOffset() + height - last_->height();
		if ( style()->alignment() == PanelLayoutStyle::RightAlignment ) last_->setPos(x + maxChildWidth - last_->width(), y);
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) last_->setPos(x + (maxChildWidth - last_->width()) / 2, y);

		// Center
		y = yOffset() + (height - middle_->height()) / 2;
		int minY = first_->pos().y() + first_->height() + style()->spaceBetweenElements();
		if ( y < minY ) y = minY;
		if ( style()->alignment() == PanelLayoutStyle::RightAlignment ) middle_->setPos(x + maxChildWidth - middle_->width(), y);
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) middle_->setPos(x + (maxChildWidth - middle_->width()) / 2, y);
	}
}

}
