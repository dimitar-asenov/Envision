/***********************************************************************************************************************
 * Layout.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/Layout.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( Layout )

Layout::Layout(Item* parent, const LayoutStyle* style) :
	Item(parent, style)
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIsFocusable, false);
	setAcceptedMouseButtons(0);
}

void Layout::setInnerSize(int width_, int height_)
{
	if ( hasShape() )
	{
		getShape()->setOffset(style()->leftMargin(), style()->topMargin());
		getShape()->setInnerSize(width_, height_);
		setWidth(width() + style()->leftMargin() + style()->rightMargin());
		setHeight(height() + style()->topMargin() + style()->bottomMargin());
	}
	else
	{
		setWidth(width_ + style()->leftMargin() + style()->rightMargin());
		setHeight(height_ + style()->topMargin() + style()->bottomMargin());
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
	if ( hasShape() ) return getShape()->contentLeft();
	else return style()->leftMargin();
}

int Layout::yOffset() const
{
	if ( hasShape() ) return getShape()->contentTop();
	else return style()->leftMargin();
}

void Layout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( hasShape() ) Item::paint(painter, option, widget);
}

}
