/***********************************************************************************************************************
 * MethodIcon.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/MethodIcon.h"

#include <QtGui/QPainter>

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(MethodIcon)

MethodIcon::MethodIcon(Item* parent, const MethodIconStyle *style) :
	Icon(parent, style)
{
}

void MethodIcon::updateGeometry(int, int)
{
	setInnerSize(style()->width(), style()->width());
}

void MethodIcon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Icon::paint(painter, option, widget);

	painter->setPen(style()->outline());
	painter->setBrush(style()->brush());
	painter->translate(style()->posFirst());
	painter->drawPath(style()->path());
	painter->translate(style()->posSecond() - style()->posFirst());
	painter->drawPath(style()->path());
}

}
