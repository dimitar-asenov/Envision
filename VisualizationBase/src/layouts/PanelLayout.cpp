/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

/***********************************************************************************************************************
 * PanelLayout.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelLayout.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( PanelLayout, "layout" )

PanelLayout::PanelLayout(Item* parent, const StyleType* style) :
	Layout(parent, style), first_(nullptr), middle_(nullptr), last_(nullptr)
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
	else if (position) position->setParentItem(nullptr);

	if ( item ) item->setParentItem(this);
	position = item;
	setUpdateNeeded();
}

bool PanelLayout::sizeDependsOnParent() const
{
	return true;
}

bool PanelLayout::isEmpty() const
{
	return ( first_ == nullptr || first_->isEmpty() )
			&& ( middle_ == nullptr || middle_->isEmpty() )
			&& (last_ == nullptr || last_->isEmpty() );
}

void PanelLayout::updateGeometry(int availableWidth, int availableHeight)
{
	QRect first, middle, last;
	if (first_) first.setSize( QSize(first_->width(), first_->height()) );
	if (middle_) middle.setSize( QSize(middle_->width(), middle_->height()) );
	if (last_) last.setSize( QSize(last_->width(), last_->height()) );


	if ( style()->orientation() == PanelLayoutStyle::HorizontalOrientation )
	{
		// Get the height
		int maxChildHeight = first.height();
		if ( middle.height() > maxChildHeight ) maxChildHeight = middle.height();
		if ( last.height() > maxChildHeight ) maxChildHeight = last.height();

		// Get the width of the area where we should put the sequential components
		int width = first.width() + middle.width() + last.width() + 2 * style()->spaceBetweenElements();

		int outterWidth = width + style()->leftMargin() + style()->rightMargin();
		if ( hasShape() ) outterWidth = getShape()->outterSize(outterWidth, maxChildHeight).width();

		if ( availableWidth > outterWidth ) width += availableWidth - outterWidth;

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
		// Get the width
		int maxChildWidth = first.width();
		if ( middle.width() > maxChildWidth ) maxChildWidth = middle.width();
		if ( last.width() > maxChildWidth ) maxChildWidth = last.width();

		// Get the height of the area where we should put the sequential components
		int height = first.height() + middle.height() + last.height() + 2 * style()->spaceBetweenElements();

		int outterHeight = height + style()->topMargin() + style()->bottomMargin();
		if ( hasShape() ) outterHeight = getShape()->outterSize(maxChildWidth, outterHeight).height();

		if ( availableHeight > outterHeight ) height += availableHeight - outterHeight;

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
		y = yOffset() + (height - middle.height()) / 2;
		int minY = first.y() + first.height() + style()->spaceBetweenElements();
		if ( y < minY ) y = minY;
		if ( style()->alignment() == PanelLayoutStyle::RightAlignment ) middle.moveTo(x + maxChildWidth - middle.width(), y);
		if ( style()->alignment() == PanelLayoutStyle::CenterAlignment ) middle.moveTo(x + (maxChildWidth - middle.width()) / 2, y);
	}

	if (first_) first_->setPos(first.topLeft());
	if (middle_) middle_->setPos(middle.topLeft());
	if (last_) last_->setPos(last.topLeft());
}

//bool PanelLayout::focusChild(FocusTarget location)
//{
//	Item* toFocus = nullptr;
//	Item* f = first_ ? first_ : (middle_ ? middle_ : last_);
//	Item* l = last_ ? last_ : (middle_ ? middle_ : first_);
//
//	if (f == nullptr) return false;
//
//	// Find out which is the current item and get the previous and next if any
//	Item* prev = nullptr;
//	Item* next = nullptr;
//	if (first_ && first_->itemOrChildHasFocus())
//	{
//		next = middle_ ? middle_ : last_;
//	}
//	else if (middle_ && middle_->itemOrChildHasFocus())
//	{
//		prev = first_;
//		next = last_;
//	}
//	else if (last_ && last_->itemOrChildHasFocus())
//	{
//		prev = middle_ ? middle_ : first_;
//	}
//
//	bool horizontal = style()->orientation() == PanelLayoutStyle::HorizontalOrientation;
//
//	switch (location)
//	{
//		case FOCUS_DEFAULT:
//			toFocus = f;
//			break;
//		case FOCUS_TOPMOST:
//			toFocus = f;
//			break;
//		case FOCUS_BOTTOMMOST:
//			toFocus = horizontal ? f : l;
//			break;
//		case FOCUS_LEFTMOST:
//			toFocus = f;
//			break;
//		case FOCUS_RIGHTMOST:
//			toFocus =  horizontal ? l : f;
//			break;
//		case FOCUS_UP:
//			if (!horizontal) toFocus = prev;
//			break;
//		case FOCUS_DOWN:
//			if (!horizontal) toFocus = next;
//			break;
//		case FOCUS_LEFT:
//			if (horizontal) toFocus = prev;
//			break;
//		case FOCUS_RIGHT:
//			if (horizontal) toFocus = next;
//			break;
//	}
//
//	return Item::focusChild(toFocus);
//}

}
