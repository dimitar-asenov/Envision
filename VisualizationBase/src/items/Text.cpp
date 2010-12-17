/***********************************************************************************************************************
 * Text.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/Text.h"

#include "Styles.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

namespace Visualization {

Text::Text(Item* parent, const QString& text_) :
	Item(parent), text(text_), style(Styles::item<Text>("distributed"))
{
}

Text::Text(Item* parent, TextStyle *style_, const QString& text_) :
	Item(parent), text(text_), style(style_)
{
}

void Text::determineChildren()
{
}

void Text::updateState()
{
	QFontMetrics qfm(style->font());

	bounding_rect = qfm.boundingRect(text.text());
	xOffset = - bounding_rect.left();
	yOffset = - bounding_rect.top();
	bounding_rect.moveTopLeft(QPointF(0,0));
	size.setWidth(bounding_rect.width());
	size.setHeight(bounding_rect.height());
}

void Text::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(style->pen());
	painter->setFont(style->font());
	painter->drawStaticText(0, 0, text);
}

}
