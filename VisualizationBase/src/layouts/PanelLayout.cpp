/***********************************************************************************************************************
 * PanelLayout.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelLayout.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( PanelLayout )

PanelLayout::PanelLayout(Item* parent, const PanelLayoutStyle* style) :
	Layout(parent, style), first_(NULL), middle_(NULL), last_(NULL)
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
	if ( item ) item->setParentItem(this);
	position = item;
	setUpdateNeeded();
}

bool PanelLayout::sizeDependsOnParent() const
{
	return true;
}

void PanelLayout::updateGeometry(int availableWidth, int availableHeight)
{
	QRect first, middle, last;
	if (first_) first.setSize( QSize(first_->width(), first_->height()) );
	if (middle_) middle.setSize( QSize(middle_->width(), middle_->height()) );
	if (last_) last.setSize( QSize(last_->width(), last_->height()) );


	if ( style()->orientation() == PanelLayoutStyle::HorizontalOrientation )
	{
		// Get the width of the area where we should put the sequential components
		int width = first.width() + middle.width() + last.width() + 2 * style()->spaceBetweenElements();

		int outterWidth = width + style()->leftMargin() + style()->rightMargin();
		if ( getShape() ) outterWidth = getShape()->getOutterWidth(outterWidth);

		if ( availableWidth > outterWidth ) width += availableWidth - outterWidth;

		// Get the height
		int maxChildHeight = first.height();
		if ( middle.height() > maxChildHeight ) maxChildHeight = middle.height();
		if ( last.height() > maxChildHeight ) maxChildHeight = last.height();

		// Set the size
		setInnerSize(width, maxChildHeight);

		// Set the position of all items
		int y = yOffset();

		// Begin
		int x = xOffset();
		if ( style()->alignment() == PanelLayoutStyle::BottomAlignment ) first.moveTo(x, y + maxChildHeight - first.height());
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) first.moveTo(x, y + (maxChildHeight - first.height()) / 2);

		// End
		x = xOffset() + width - last.width();
		if ( style()->alignment() == PanelLayoutStyle::BottomAlignment ) last.moveTo(x, y + maxChildHeight - last.height());
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) last.moveTo(x, y + (maxChildHeight - last.height()) / 2);

		// Center
		x = xOffset() + (width - middle.width()) / 2;
		int minX = first.x() + first.width() + style()->spaceBetweenElements();
		if ( x < minX ) x = minX;
		if ( style()->alignment() == PanelLayoutStyle::BottomAlignment ) middle.moveTo(x, y + maxChildHeight - middle.height());
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) middle.moveTo(x, y + (maxChildHeight - middle.height()) / 2);
	}
	else
	{
		// Get the height of the area where we should put the sequential components
		int height = first.height() + middle.height() + last.height() + 2 * style()->spaceBetweenElements();

		int outterHeight = height + style()->topMargin() + style()->bottomMargin();
		if ( getShape() ) outterHeight = getShape()->getOutterHeight(outterHeight);

		if ( availableHeight > outterHeight ) height += availableHeight - outterHeight;

		// Get the width
		int maxChildWidth = first.width();
		if ( middle.width() > maxChildWidth ) maxChildWidth = middle.width();
		if ( last.width() > maxChildWidth ) maxChildWidth = last.width();

		// Set the size
		setInnerSize(maxChildWidth, height);

		// Set the position of all items
		int x = xOffset();

		// Begin
		int y = yOffset();
		if ( style()->alignment() == PanelLayoutStyle::RightAlignment ) first.moveTo(x + maxChildWidth - first.width(), y);
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) first.moveTo(x + (maxChildWidth - first.width()) / 2, y);

		// End
		y = yOffset() + height - last.height();
		if ( style()->alignment() == PanelLayoutStyle::RightAlignment ) last.moveTo(x + maxChildWidth - last.width(), y);
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) last.moveTo(x + (maxChildWidth - last.width()) / 2, y);

		// Center
		y = yOffset() + (height - middle_->height()) / 2;
		int minY = first.y() + first.height() + style()->spaceBetweenElements();
		if ( y < minY ) y = minY;
		if ( style()->alignment() == PanelLayoutStyle::RightAlignment ) middle.moveTo(x + maxChildWidth - middle.width(), y);
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) middle.moveTo(x + (maxChildWidth - middle.width()) / 2, y);
	}

	if (first_) first_->setPos(first.topLeft());
	if (middle_) middle_->setPos(middle.topLeft());
	if (last_) last_->setPos(last.topLeft());
}

}
