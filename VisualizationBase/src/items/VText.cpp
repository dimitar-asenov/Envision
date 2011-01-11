/***********************************************************************************************************************
 * VText.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VText.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VText)

VText::VText(Item* parent, Model::Text *text, const TextStyle *style) :
	ModelItem(parent, text, style)
{
}

void VText::determineChildren()
{
}

void VText::updateState()
{
	Model::Text* textNode = static_cast<Model::Text*> (getNode());

	text = textNode->get();
	QFontMetrics qfm(style()->font());

	bounding_rect = qfm.boundingRect(text);
	xOffset = - bounding_rect.left();
	yOffset = - bounding_rect.top();
	bounding_rect.moveTopLeft(QPointF(0,0));
	size.setWidth(bounding_rect.width());
	size.setHeight(bounding_rect.height());
}

void VText::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(style()->pen());
	painter->setFont(style()->font());
	painter->drawText(xOffset, yOffset, text);
}

}
