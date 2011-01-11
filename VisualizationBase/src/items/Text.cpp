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
ITEM_COMMON_DEFINITIONS(Text)

Text* Text::selected = NULL;
int Text::selectionBegin = 0;
int Text::selectionEnd = 0;
int Text::selectionXBegin = 0;
int Text::selectionXEnd = 0;
int Text::caretX = 0;

Text::Text(Item* parent, const QString& text_) :
	Item(parent, Styles::item<Text>("default")), text(text_)
{
}

Text::Text(Item* parent, const TextStyle *style_, const QString& text_) :
	Item(parent, style_), text(text_)
{
}

void Text::determineChildren()
{
}

void Text::updateState()
{
	setFlag(QGraphicsItem::ItemHasNoContents, false);
	QFontMetrics qfm(style()->font());

	bounding_rect = qfm.boundingRect(text);
	xOffset = -bounding_rect.left();
	yOffset = -bounding_rect.top();
	bounding_rect.moveTopLeft(QPointF(0, 0));
	size.setWidth(bounding_rect.width());
	size.setHeight(bounding_rect.height());

	if ( selected == this )
	{
		int xstart = selectionBegin;
		int xend = selectionEnd;
		if ( selectionBegin > selectionEnd )
		{
			xstart = selectionEnd;
			xend = selectionBegin;
		}

		selectionXBegin = qfm.width(text, xstart);
		selectionXEnd = qfm.width(text, xend+1);
		caretX = (selectionBegin > selectionEnd) ? selectionXBegin : selectionXEnd;
	}
}

void Text::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if (selected != this)
	{
		painter->setPen(style()->pen());
		painter->setFont(style()->font());
		painter->drawText(xOffset, yOffset, text);
	}
	else
	{
		int xstart = selectionBegin;
		int xend = selectionEnd;
		if ( xstart > xend )
		{
			xstart = selectionEnd;
			xend = selectionBegin;
		}

		painter->setPen(style()->selectionPen());
		painter->setFont(style()->selectionFont());
		painter->drawText(xOffset + selectionXBegin, yOffset, text.mid(xstart, xend - xstart+1));

		painter->setPen(style()->pen());
		painter->setFont(style()->font());
		painter->drawText(xOffset, yOffset, text.left(xstart));
		painter->drawText(xOffset + selectionXEnd, yOffset, text.mid(xend+1));
		painter->drawLine(caretX, bounding_rect.top()+1, caretX, bounding_rect.bottom()-1);
	}
}

}
