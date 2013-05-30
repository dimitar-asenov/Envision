/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "layouts/PanelBorderLayout.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( PanelBorderLayout, "layout" )

PanelBorderLayout::PanelBorderLayout(Item* parent, const StyleType* style) :
Super(parent, style), top_(new PanelLayout(this, &style->topStyle())),
	left_(), bottom_(), right_(), content_()
{
}

PanelBorderLayout::~PanelBorderLayout()
{
	SAFE_DELETE_ITEM(top_);
	SAFE_DELETE_ITEM(left_);
	SAFE_DELETE_ITEM(bottom_);
	SAFE_DELETE_ITEM(right_);
	SAFE_DELETE_ITEM(content_);
}

void PanelBorderLayout::setPanel(bool enable, PanelLayout*& panel, const PanelLayoutStyle& style)
{
	if ( enable && !panel ) panel = new PanelLayout(this, &style);
	if ( !enable && panel ) SAFE_DELETE_ITEM(panel);
	setUpdateNeeded(StandardUpdate);
}

void PanelBorderLayout::setContent(Item* content, bool deleteOldContent)
{
	if (content_ == content) return;

	if ( deleteOldContent ) SAFE_DELETE_ITEM(content_);
	else if (content_) content_->setParentItem(nullptr);

	if ( content ) content->setParentItem(this);
	content_ = content;
	setUpdateNeeded(StandardUpdate);
}

bool PanelBorderLayout::isEmpty() const
{
	return ( top_ == nullptr || top_->isEmpty())
			&& ( left_ == nullptr || left_->isEmpty())
			&& ( content_ == nullptr || content_->isEmpty())
			&& ( right_ == nullptr || right_->isEmpty())
			&& ( bottom_ == nullptr || bottom_->isEmpty());
}

void PanelBorderLayout::updateGeometry(int, int)
{
	//TODO: Find a generic way of doing this and consider performance
	//Set Styles
	if (top_) top_->setStyle(&style()->topStyle());
	if (left_) left_->setStyle(&style()->leftStyle());
	if (right_) right_->setStyle(&style()->rightStyle());
	if (bottom_) bottom_->setStyle(&style()->bottomStyle());

	// Get content size
	int contentWidth = content_ ? content_->width() : 0;
	int contentHeight = content_ ? content_->height() : 0;

	if (hasShape() && style()->shapeOnlyOnContent() && getShape()->style()->outline() != Qt::NoPen)
	{
		int shapeOutline = getShape()->style()->outline().width();
		contentWidth += shapeOutline;
		contentHeight += shapeOutline;
	}

	// Compute middle sizes
	int middleWidth = contentWidth + style()->leftInnerMargin() + style()->rightInnerMargin();
	if ( left_ && !style()->isLeftProtrusionFixed()) middleWidth += left_->width() / 2;
	if ( right_ ) middleWidth += right_->width() / 2;

	if (style()->isLeftProtrusionFixed())
	{
		int extra = (left_ ? left_->width() : 0) - style()->leftProtrusion();
		if (extra > 0) middleWidth += extra;
	}

	int maxMiddleWidth = middleWidth;
	if ( top_ && top_->width() > maxMiddleWidth ) maxMiddleWidth = top_->width();
	if ( bottom_ && bottom_->width() > maxMiddleWidth ) maxMiddleWidth = bottom_->width();

	if ( left_ && left_->height() > contentHeight ) contentHeight = left_->height();
	if ( right_ && right_->height() > contentHeight ) contentHeight = right_->height();

	//Adjust the size of the content if necessary
	if (content_ && content_->sizeDependsOnParent())
		content_->changeGeometry(contentWidth + (maxMiddleWidth - middleWidth), contentHeight);

	//Adjust panels and/or the inner size
	if ( maxMiddleWidth > middleWidth ) contentWidth += maxMiddleWidth - middleWidth;
	if ( top_ && maxMiddleWidth > top_->width() ) top_->changeGeometry(maxMiddleWidth, 0);
	if ( bottom_ && maxMiddleWidth > bottom_->width() ) bottom_->changeGeometry(maxMiddleWidth, 0);
	if ( left_ && contentHeight > left_->height() ) left_->changeGeometry(0, contentHeight);
	if ( right_ && contentHeight > right_->height() ) right_->changeGeometry(0, contentHeight);

	// Compute outter sizes
	int outterWidth = contentWidth + style()->leftInnerMargin() + style()->rightInnerMargin();

	int extraLeft = 0;
	if ( style()->isLeftProtrusionFixed() && style()->leftProtrusion() > 0) extraLeft = style()->leftProtrusion();
	if ( left_ && left_->width() > extraLeft) extraLeft = left_->width();
	outterWidth += extraLeft;

	if ( right_ ) outterWidth += right_->width();
	int outterHeight = contentHeight + style()->topInnerMargin() + style()->bottomInnerMargin();
	if ( top_ ) outterHeight += top_->height();
	if ( bottom_ ) outterHeight += bottom_->height();


	if (hasShape() && style()->shapeOnlyOnContent())
	{
		// If the shape must cover only the content manually set the way it should look.

		// Set shape position
		int x = style()->leftMargin();
		if (style()->isLeftProtrusionFixed())
		{
			if (style()->leftProtrusion() > 0) x += style()->leftProtrusion();
		}
		else x += left_ ? left_->width() / 2 : 0;

		int y = style()->topMargin() + (top_ ? top_->height() / 2 : 0);
		getShape()->setOffset(x, y);

		// Set shape size
		int middleHeight = outterHeight - y - (bottom_? (bottom_->height()/2) :0);
		getShape()->setOutterSize(maxMiddleWidth, middleHeight);

		// Set Layout size
		setWidth(outterWidth + style()->leftMargin() + style()->rightMargin());
		setHeight(outterHeight + style()->topMargin() + style()->bottomMargin());
	}
	else setInnerSize(outterWidth, outterHeight); // Use the default method when the shape must cover the entire layout.

	// Determine the right offset to use.
	int xOffset = style()->leftMargin();
	int yOffset = style()->rightMargin();
	if (hasShape() && !style()->shapeOnlyOnContent())
	{
		xOffset = getShape()->contentLeft();
		yOffset = getShape()->contentTop();
	}

	// Set positions
	int xLeft = xOffset;
	int xTop = xLeft;
	int xContent;
	int xRight;

	if (style()->isLeftProtrusionFixed())
	{
		if ( style()->leftProtrusion() < 0) xLeft -= style()->leftProtrusion();
		else xTop += style()->leftProtrusion();

		xContent = xLeft + (left_ ? left_->width() : 0) + style()->leftInnerMargin();
		if (xContent < xOffset + style()->leftProtrusion() + style()->leftInnerMargin())
			xContent = xOffset + style()->leftProtrusion() + style()->leftInnerMargin();
	}
	else
	{
		xTop = xLeft + (left_ ? left_->width() / 2 : 0);
		xContent = xLeft + (left_ ? left_->width() : 0) + style()->leftInnerMargin();
	}

	xRight = xContent + (content_ ? content_->width() : 0) + style()->rightInnerMargin();

	int yTop = yOffset;
	int yContent = yTop + (top_ ? top_->height() : 0) + style()->topInnerMargin();
	int yBottom = yContent + contentHeight + style()->bottomInnerMargin();

	if ( top_ ) top_->setPos(xTop, yTop);
	if ( left_ ) left_->setPos(xLeft, yContent);
	if ( content_ ) content_->setPos(xContent, yContent);
	if ( right_ ) right_->setPos(xRight, yContent);
	if ( bottom_ ) bottom_->setPos(xTop, yBottom);
}

}
