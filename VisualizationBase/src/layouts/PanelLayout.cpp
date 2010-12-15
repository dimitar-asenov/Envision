/***********************************************************************************************************************
 * PanelLayout.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelLayout.h"
#include "items/NoItem.h"

namespace Visualization {

PanelLayout::PanelLayout(Item* parent, PanelLayoutStyle* style_) :
	Layout(parent), style(style_), first_(new NoItem(this)), middle_(new NoItem(this)), last_(new NoItem(this))
{
}

void PanelLayout::setStyle(PanelLayoutStyle* style_)
{
	style = style_;
	setUpdateNeeded();
}

void PanelLayout::updateState()
{
	if ( style->orientation() == PanelLayoutStyle::HorizontalOrientation )
	{
		// Get the width of the area where we should put the sequential components
		int width = style->leftMargin() + first_->width() + middle_->width() + last_->width() + 2 * style->spaceBetweenElements() + style->rightMargin();

		int outterWidth = width;
		if ( getShape() ) outterWidth = getShape()->getOutterWidth(width);

		if ( minimalLength > outterWidth ) width += minimalLength - outterWidth;

		// Get the height
		int maxChildHeight = first_->height();
		if (middle_->height() > maxChildHeight) maxChildHeight = middle_->height();
		if (last_->height() > maxChildHeight) maxChildHeight = last_->height();

		// Set the size
		setInnerSize(width, maxChildHeight + style->topMargin() + style->bottomMargin());

		// Set the position of all items
		int y = style->topMargin() + yOffset();

		// Begin
		int x = xOffset() + style->leftMargin();
		if ( style->alignment() == PanelLayoutStyle::BottomAlignment ) first_->setPos(x, y + maxChildHeight - first_->height());
		if ( style->alignment() == PanelLayoutStyle::CenterAlignment ) first_->setPos(x, y + (maxChildHeight - first_->height()) / 2);

		// End
		x = width - style->rightMargin() - last_->width();
		if ( style->alignment() == PanelLayoutStyle::BottomAlignment ) last_->setPos(x, y + maxChildHeight - last_->height());
		if ( style->alignment() == PanelLayoutStyle::CenterAlignment ) last_->setPos(x, y + (maxChildHeight - last_->height()) / 2);

		// Center
		x = (width - middle_->width() )/ 2;
		int minX = first_->pos().x() + first_->width() + style->spaceBetweenElements();
		if (x < minX ) x = minX;
		if ( style->alignment() == PanelLayoutStyle::BottomAlignment ) middle_->setPos(x, y + maxChildHeight - middle_->height());
		if ( style->alignment() == PanelLayoutStyle::CenterAlignment ) middle_->setPos(x, y + (maxChildHeight - middle_->height()) / 2);
	}
	else
	{
		// Get the height of the area where we should put the sequential components
		int height = style->topMargin() + first_->height() + middle_->height() + last_->height() + 2 * style->spaceBetweenElements() + style->bottomMargin();

		int outterHeight = height;
		if ( getShape() ) outterHeight = getShape()->getOutterHeight(height);

		if ( minimalLength > outterHeight ) height += minimalLength - outterHeight;

		// Get the width
		int maxChildWidth = first_->width();
		if (middle_->width() > maxChildWidth) maxChildWidth = middle_->width();
		if (last_->width() > maxChildWidth) maxChildWidth = last_->width();

		// Set the size
		setInnerSize( maxChildWidth + style->leftMargin() + style->rightMargin(), height);

		// Set the position of all items
		int x = style->leftMargin() + xOffset();

		// Begin
		int y = yOffset() + style->topMargin();
		if ( style->alignment() == PanelLayoutStyle::RightAlignment ) first_->setPos(x + maxChildWidth - first_->width(), y);
		if ( style->alignment() == PanelLayoutStyle::CenterAlignment ) first_->setPos(x + (maxChildWidth - first_->width()) / 2, y);

		// End
		y = height - style->bottomMargin() - last_->height();
		if ( style->alignment() == PanelLayoutStyle::RightAlignment ) last_->setPos(x + maxChildWidth - last_->width(), y);
		if ( style->alignment() == PanelLayoutStyle::CenterAlignment ) last_->setPos(x + (maxChildWidth - last_->width()) / 2, y);

		// Center
		y = (height - middle_->height() )/ 2;
		int minY = first_->pos().y() + first_->height() + style->spaceBetweenElements();
		if (y < minY ) y = minY;
		if ( style->alignment() == PanelLayoutStyle::RightAlignment ) middle_->setPos(x + maxChildWidth - middle_->width(), y);
		if ( style->alignment() == PanelLayoutStyle::CenterAlignment ) middle_->setPos(x + (maxChildWidth - middle_->width()) / 2, y);
	}
}

}
