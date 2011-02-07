/***********************************************************************************************************************
 * ClassIcon.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/ClassIcon.h"

#include <QtGui/QPainter>

#include <cmath>

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

	int width = style()->width();
	int halfWidth = width / 2;

	QRadialGradient gradient(halfWidth, halfWidth, halfWidth);
	gradient.setColorAt(0, Qt::white);
	gradient.setColorAt(0.3, Qt::white);
	gradient.setColorAt(1, Qt::transparent);
	painter->setBrush(QBrush(gradient));
	painter->setPen(Qt::NoPen);
	painter->drawRect(xOffset(), yOffset(), style()->width(), style()->width());

	painter->setBrush(Qt::white);
	painter->setPen( style()->outline());

	qreal x = xOffset();
	qreal y = yOffset();

	// Move the figure when using antialiasing. The outline will start at a pixel boundary. This makes it sharper.
	qreal outlineWidth = style()->outline().width();
	if ( painter->testRenderHint(QPainter::Antialiasing) || painter->testRenderHint(QPainter::HighQualityAntialiasing) )
		if ( style()->outline() != Qt::NoPen )
		{
			qreal intPart;
			qreal fracPart = std::modf(outlineWidth / 2.0, &intPart);

			x += fracPart;
			y += fracPart;
		}

	int restX = (halfWidth - style()->rectWidth())/2;
	int restY = (halfWidth - style()->rectHeight())/2;

	// Draw the lines exactly in the middle. These are independent of how big the rectangles are.
	painter->drawLine(QLineF(x + halfWidth / 2, y + halfWidth, x + halfWidth*3 / 2, y + halfWidth));
	painter->drawLine(QLineF(x + halfWidth, y + halfWidth, x + halfWidth, y+ halfWidth/2));
	painter->drawLine(QLineF(x + halfWidth/2, y + halfWidth, x + halfWidth/2, y + halfWidth*3/2));
	painter->drawLine(QLineF(x + halfWidth*3/2, y + halfWidth, x + halfWidth*3/2, y + halfWidth*3/2));

	painter->drawRect(QRectF(x + ((width - style()->rectWidth()) / 2), y + restY, style()->rectWidth(), style()->rectHeight()));
	painter->drawRect(QRectF(x + restX, y + halfWidth + restY, style()->rectWidth(), style()->rectHeight()));
	painter->drawRect(QRectF(x + halfWidth + restX, y + halfWidth + restY, style()->rectWidth(), style()->rectHeight()));



}

}
