/***********************************************************************************************************************
 * Layout.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/Layout.h"

namespace Visualization {

Layout::Layout(Item* parent, const LayoutStyle* style) :
	Item(parent, style)
{
}

void Layout::setInnerSize(int width_, int height_)
{
	if ( getShape() )
	{
		getShape()->setOffset(style()->leftMargin(), style()->topMargin());
		getShape()->setInnerSize(width_, height_);
		size.setWidth(width() + style()->leftMargin() + style()->rightMargin());
		size.setHeight(height() + style()->topMargin() + style()->bottomMargin());
	}
	else
	{
		bounding_rect = QRectF();
		size.setWidth(width_ + style()->leftMargin() + style()->rightMargin());
		size.setHeight(height_ + style()->topMargin() + style()->bottomMargin());
	}

}

void Layout::determineChildren()
{
}

bool Layout::needsUpdate()
{
	QList<QGraphicsItem *> children = childItems();
	for (QList<QGraphicsItem *>::iterator child = children.begin(); child != children.end(); ++child)
	{
		Item* item = static_cast<Item*> (*child);
		if ( item->needsUpdate() ) return true;
	}

	return false;
}

int Layout::xOffset() const
{
	if ( getShape() ) return getShape()->contentLeft();
	else return style()->leftMargin();
}

int Layout::yOffset() const
{
	if ( getShape() ) return getShape()->contentTop();
	else return style()->leftMargin();
}

void Layout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( getShape() ) Item::paint(painter, option, widget);
}

}
