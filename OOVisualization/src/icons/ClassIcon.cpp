/***********************************************************************************************************************
 * ClassIcon.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/ClassIcon.h"

#include <QtGui/QPainter>

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(ClassIcon)

ClassIcon::ClassIcon(Item* parent, const ClassIconStyle *style) :
	Icon(parent, style)
{
}

void ClassIcon::updateGeometry(int, int)
{
	setInnerSize(style()->width(), style()->width());
}

void ClassIcon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Icon::paint(painter, option, widget);

	int halfWidth = style()->width() / 2;

	QRadialGradient gradient(halfWidth, halfWidth, halfWidth, halfWidth, halfWidth);
	gradient.setColorAt(0, Qt::white);
	gradient.setColorAt(1, Qt::transparent);
	painter->setBrush(QBrush(gradient));
	painter->setPen(Qt::NoPen);
	painter->drawRect(xOffset(), yOffset(), style()->width(), style()->width());

	painter->setBrush(Qt::white);

	painter->setPen( style()->outline());

	// TODO: Fix the random magic numbers below
	int contentWidth = style()->width() / 2 - 2*style()->outline().width();
	int contentHeight = style()->width() / 3;

	painter->drawRect(xOffset() + (style()->width() - contentWidth) / 2, yOffset() + 2, contentWidth, contentHeight);
	painter->drawRect(xOffset() + 1, yOffset() + style()->width() - 2 - contentHeight, contentWidth, contentHeight);
	painter->drawRect(xOffset() + style()->width() - 1 - contentWidth, yOffset() + style()->width() - 2 - contentHeight, contentWidth, contentHeight);

	painter->drawLine(xOffset() + 1 + contentWidth / 2, yOffset() + halfWidth, xOffset() + style()->width() - 1 - contentWidth / 2, yOffset() + halfWidth);
	painter->drawLine(xOffset() + halfWidth, yOffset() + 2 + contentHeight, xOffset() + halfWidth, halfWidth);
	painter->drawLine(xOffset() + 1 + contentWidth / 2, yOffset() + halfWidth, xOffset() + 1 + contentWidth / 2, yOffset() + style()->width() - 2 - contentHeight);
	painter->drawLine(xOffset() + style()->width() - 1 - contentWidth / 2, yOffset() + halfWidth, xOffset() + style()->width() - 1 - contentWidth / 2, yOffset() + style()->width() - 2 - contentHeight);

}

}
