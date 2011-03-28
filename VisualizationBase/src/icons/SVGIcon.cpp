/***********************************************************************************************************************
 * SVGIcon.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/SVGIcon.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(SVGIcon, "icon")

SVGIcon::SVGIcon(Item* parent, const QString& iconStyleName) :
	Icon(parent, itemStyles().get(iconStyleName))
{
}

SVGIcon::SVGIcon(Item* parent, const SVGIconStyle *style) :
	Icon(parent, style)
{
}

void SVGIcon::updateGeometry(int, int)
{
	setInnerSize(style()->width(), style()->height());
}

void SVGIcon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Icon::paint(painter, option, widget);

	style()->paint(painter, xOffset(), yOffset() );
}

}
