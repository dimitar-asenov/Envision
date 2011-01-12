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
		selectionXEnd = qfm.width(text, xend);
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
		if (selectionXBegin == selectionXEnd)
		{
			// No text is selected, draw all text at once using normal style
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawText(xOffset, yOffset, text);
		}
		else
		{
			// Some text is selected, draw it differently than non-selected text.

			int xstart = selectionBegin;
			int xend = selectionEnd;
			if ( xstart > xend )
			{
				xstart = selectionEnd;
				xend = selectionBegin;
			}

			// Draw selected text
			painter->setPen(style()->selectionPen());
			painter->setFont(style()->selectionFont());
			painter->drawText(xOffset + selectionXBegin, yOffset, text.mid(xstart, xend - xstart));

			// Draw non-selected text
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawText(xOffset, yOffset, text.left(xstart));
			painter->drawText(xOffset + selectionXEnd, yOffset, text.mid(xend));
		}

		// Draw caret
		painter->setPen(style()->caretPen());
		painter->drawLine(xOffset + caretX, bounding_rect.top()+1, xOffset + caretX, bounding_rect.bottom()-1);
	}
}

void Text::setSelected(int xBegin, int xEnd)
{
	selectionBegin = 0;
	selectionEnd = 0;

	QFontMetrics qfm(style()->font());
	int width = 0;
	for (int i = 1; i<= text.length(); ++i)
	{
		int new_width = qfm.width(text, i);
		if (xBegin > (new_width + width + 1 ) / 2 ) selectionBegin++;
		if (xEnd   > (new_width + width + 1 ) / 2 ) selectionEnd++;
		width = new_width;
	}

	resetSelected();
	selected = this;
	setUpdateNeeded();
}

void Text::resetSelected()
{
	if (selected) selected->setUpdateNeeded();
	selected = NULL;
}

}
