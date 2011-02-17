/***********************************************************************************************************************
 * Layout.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/Layout.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( Layout )

Layout::Layout(Item* parent, const StyleType* style) :
	Item(parent, style)
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	//setFlag(QGraphicsItem::ItemIsFocusable, false);
	setAcceptedMouseButtons(0);
}

void Layout::setInnerSize(int width_, int height_)
{
	if (isEmpty() && !style()->drawShapeWhenEmpty()) setSize(0,0);
	else
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
}

void Layout::determineChildren()
{
}

int Layout::xOffset() const
{
	if ( hasShape() ) return getShape()->contentLeft();
	else return style()->leftMargin();
}

int Layout::yOffset() const
{
	if ( hasShape() ) return getShape()->contentTop();
	else return style()->topMargin();
}

void Layout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( hasShape() && (style()->drawShapeWhenEmpty() || !isEmpty()) ) Item::paint(painter, option, widget);
}

}
