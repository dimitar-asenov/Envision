/***********************************************************************************************************************
 * PanelBorderLayout.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelBorderLayout.h"

#include <QtCore/QDebug>

namespace Visualization {

PanelBorderLayout::PanelBorderLayout(Item* parent, PanelBorderLayoutStyle* style_) :
	Layout(parent), style(style_), top_(new PanelLayout(this, &style->topStyle())), left_(NULL), bottom_(NULL), right_(NULL), content_(NULL)
{
}

const PanelBorderLayoutStyle* PanelBorderLayout::getStyle() const
{
	return style;
}

void PanelBorderLayout::setStyle(PanelBorderLayoutStyle* style_)
{
	style = style_;
	setUpdateNeeded();
}

void PanelBorderLayout::setPanel(bool enable, PanelLayout*& panel, PanelLayoutStyle& style)
{
	if ( enable && !panel ) panel = new PanelLayout(this, &style);
	if ( !enable && panel )
	{
		delete panel;
		panel = NULL;
	}
	setUpdateNeeded();
}

void PanelBorderLayout::setContent(Item* content)
{
	if ( content_ ) delete content_;
	if ( content ) content->setParentItem(this);
	content_ = content;
	setUpdateNeeded();
}

void PanelBorderLayout::updateState()
{
	// Get content size
	int innerWidth = content_ ? content_->width() : 0;
	int innerHeight = content_ ? content_->height() : 0;

	// Compute middle sizes
	int middleWidth = innerWidth + style->leftInnerMargin() + style->rightInnerMargin();
	if ( left_ ) middleWidth += left_->width() / 2;
	if ( right_ ) middleWidth += right_->width() / 2;

	int maxMiddleWidth = middleWidth;
	if ( top_ && top_->width() > maxMiddleWidth ) maxMiddleWidth = top_->width();
	if ( bottom_ && bottom_->width() > maxMiddleWidth ) maxMiddleWidth = bottom_->width();

	if ( left_ && left_->height() > innerHeight ) innerHeight = left_->height();
	if ( right_ && right_->height() > innerHeight ) innerHeight = right_->height();

	//Adjust panels and/or the inner size
	if ( maxMiddleWidth > middleWidth ) innerWidth += maxMiddleWidth - middleWidth;
	if ( top_ && maxMiddleWidth > top_->width() )
	{
		top_->setMinimalLength(maxMiddleWidth);
		top_->updateSubtreeState();
	}
	if ( bottom_ && maxMiddleWidth > bottom_->width() )
	{
		bottom_->setMinimalLength(maxMiddleWidth);
		bottom_->updateSubtreeState();
	}

	if ( left_ && innerHeight > left_->height() )
	{
		left_->setMinimalLength(innerHeight);
		left_->updateSubtreeState();
	}
	if ( right_ && innerHeight > right_->height() )
	{
		right_->setMinimalLength(innerHeight);
		right_->updateSubtreeState();
	}

	// Compute outter sizes
	int outterWidth = innerWidth + style->leftInnerMargin() + style->rightInnerMargin();
	if ( left_ ) outterWidth += left_->width();
	if ( right_ ) outterWidth += right_->width();
	int outterHeight = innerHeight + style->topInnerMargin() + style->bottomInnerMargin();
	if ( top_ ) outterHeight += top_->height();
	if ( bottom_ ) outterHeight += bottom_->height();

	// Set offsets and sizes
	int x = style->leftMargin();
	int y = style->topMargin();

	if ( !getShape() || style->shapeBorder() == LayoutStyle::OutterBorder )
	{
		setInnerSize(outterWidth, outterHeight);
		x = xOffset();
		y = yOffset();
	}
	else if ( style->shapeBorder() == LayoutStyle::MiddleBorder )
	{
		getShape()->setOffset(x + (left_ ? left_->width() / 2 : 0), y + (top_ ? top_->height() / 2 : 0));

		int shapeHeight = innerHeight + style->topInnerMargin() + style->bottomInnerMargin();
		if ( top_ ) shapeHeight += top_->height() / 2;
		if ( bottom_ ) shapeHeight += bottom_->height() / 2;
		getShape()->setOutterSize(maxMiddleWidth, shapeHeight);

		size.setWidth(outterWidth + style->leftMargin() + style->rightMargin());
		size.setHeight(outterHeight + style->topMargin() + style->bottomMargin());

		qDebug() << shapeHeight << " " << outterHeight << " " << height() << " " << style->topMargin() << " " << style->bottomMargin();
	}

	// Set positions
	int y2 = y + (top_ ? top_->height() : 0) + style->topInnerMargin();
	int y3 = y2 + innerHeight + style->bottomInnerMargin();

	int x2 = x + (left_ ? left_->width() / 2 : 0);
	int x3 = x + style->leftInnerMargin() + (left_ ? left_->width() : 0);
	int x4 = x3 + style->rightInnerMargin() + ( content_ ? content_->width() : 0);

	if (top_) top_->setPos(x2,y);
	if (left_) left_->setPos(x,y2);
	if (content_) content_->setPos(x3,y2);
	if (right_) right_->setPos(x4,y2);
	if (bottom_) bottom_->setPos(x2,y3);

	qDebug() << content_->height();
	qDebug() << content_->y() << " " << content_->y() + content_->height() << " " << height() - 2;
}

}
