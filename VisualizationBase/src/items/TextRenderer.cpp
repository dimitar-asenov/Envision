/***********************************************************************************************************************
 * TextRenderer.cpp
 *
 *  Created on: Jan 12, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/TextRenderer.h"

#include "items/ItemWithNode.h"
#include "Scene.h"
#include "VisualizationException.h"
#include "shapes/Shape.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

namespace Visualization {

ITEM_COMMON_DEFINITIONS(TextRenderer, "item")

const int MIN_TEXT_WIDTH = 10;

int TextRenderer::selectionBegin = 0;
int TextRenderer::selectionEnd = 0;
int TextRenderer::selectionXBegin = 0;
int TextRenderer::selectionXEnd = 0;
int TextRenderer::caretX = 0;

TextRenderer::TextRenderer(Item* parent, const StyleType *style, const QString& text) :
	Item(parent, style), text_(text), editable(true)
{
}

bool TextRenderer::setText(const QString& newText)
{
	text_ = newText;
	this->setUpdateNeeded();
	return true;
}

QString TextRenderer::selectedText()
{
	if (this->hasFocus())
	{
		int xstart = selectionBegin;
		int xend = selectionEnd;
		if ( xstart > xend )
		{
			xstart = selectionEnd;
			xend = selectionBegin;
		}

		return text_.mid(xstart, xend - xstart);
	}
	else return QString();
}

void TextRenderer::determineChildren()
{
}

void TextRenderer::updateGeometry(int, int)
{
	text_ = currentText();
	QFontMetrics qfm(style()->font());

	QRectF bound;
	if (text_.isEmpty()) bound.setRect(0, 0, MIN_TEXT_WIDTH, qfm.height());
	else
	{
		bound = qfm.boundingRect(text_);
		if (bound.width() < qfm.width(text_)) bound.setWidth(qfm.width(text_));
		if (bound.height() < qfm.height()) bound.setHeight(qfm.height());
	}

	if (this->hasShape())
	{
		this->getShape()->setInnerSize(bound.width(), bound.height());
		xOffset = -bound.left() + this->getShape()->contentLeft();
		yOffset = -bound.top() + this->getShape()->contentTop();
	}
	else
	{
		xOffset = -bound.left();
		yOffset = -bound.top();
		this->setSize(bound.size());
	}

	// Correct underline, otherwise it is drawn in the middle of two pixels and appears fat and transparent.
	if (style()->font().underline() && qfm.lineWidth() % 2)
	{
		xOffset += 0.5;
		yOffset += 0.5;
	}

	if ( this->hasFocus() )
	{
		int xstart = selectionBegin;
		int xend = selectionEnd;
		if ( selectionBegin > selectionEnd )
		{
			xstart = selectionEnd;
			xend = selectionBegin;
		}

		selectionXBegin = qfm.width(text_, xstart);
		selectionXEnd = qfm.width(text_, xend);
		caretX = (selectionBegin > selectionEnd) ? selectionXBegin : selectionXEnd;
	}
}

void TextRenderer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Item::paint(painter, option, widget);

	//int numSelected = this->scene()->selectedItems().size();

	if ( !this->hasFocus() /*|| numSelected > 1  || (numSelected == 1 && !this->isSelected())*/)
	{
		painter->setPen(style()->pen());
		painter->setFont(style()->font());
		painter->drawText(QPointF(xOffset, yOffset), text_);
	}
	else
	{
		if ( selectionXBegin == selectionXEnd /*|| numSelected == 1*/ )
		{
			// No text is selected, draw all text at once using normal style
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawText(QPointF(xOffset, yOffset), text_);
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

			// Draw selection background
			painter->setPen(Qt::NoPen);
			painter->setBrush(style()->selectionBackground());
			painter->drawRect(xOffset + selectionXBegin, 0, selectionXEnd - selectionXBegin, this->height());
			painter->setBrush(Qt::NoBrush);

			// Draw selected text
			painter->setPen(style()->selectionPen());
			painter->setFont(style()->selectionFont());
			painter->drawText(QPointF(xOffset + selectionXBegin, yOffset), text_.mid(xstart, xend - xstart));

			// Draw non-selected text
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawText(xOffset, yOffset, text_.left(xstart));
			painter->drawText(QPointF(xOffset + selectionXEnd, yOffset), text_.mid(xend));
		}

		// Draw caret
		if ( (/*selectionXBegin == selectionXEnd &&*/ editable) /*|| numSelected == 0*/)
		{
			painter->setPen(style()->caretPen());
			painter->drawLine(xOffset + caretX, 1, xOffset + caretX, this->height() - 1);
		}
	}
}

void TextRenderer::selectAll()
{
	selectionBegin = 0;
	selectionEnd = text_.length();
	if (!this->hasFocus()) this->setFocus();
	this->setUpdateNeeded();
}

void TextRenderer::setSelectedCharacters(int first, int last)
{
	selectionBegin = first;
	selectionEnd = last;
	if (!this->hasFocus()) this->setFocus();
	this->setUpdateNeeded();
}

void TextRenderer::setSelectedByDrag(int xBegin, int xEnd)
{
	selectionBegin = 0;
	selectionEnd = 0;

	QFontMetrics qfm(style()->font());
	int width = 0;
	for (int i = 1; i <= text_.length(); ++i)
	{
		int new_width = qfm.width(text_, i);
		if ( xBegin > (new_width + width + 1) / 2 ) selectionBegin++;
		if ( xEnd > (new_width + width + 1) / 2 ) selectionEnd++;
		width = new_width;
	}

	this->setFocus();
	this->setUpdateNeeded();
}

}
