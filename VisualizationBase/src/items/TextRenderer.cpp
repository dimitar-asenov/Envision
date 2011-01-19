/***********************************************************************************************************************
 * TextRenderer.cpp
 *
 *  Created on: Jan 12, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/TextRenderer.h"

#include "items/ModelItem.h"
#include "VisualizationException.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

namespace Visualization {

const int MIN_TEXT_WIDTH = 10;

template<class T> TextRenderer<T>* TextRenderer<T>::selected = NULL;
template<class T> int TextRenderer<T>::selectionBegin = 0;
template<class T> int TextRenderer<T>::selectionEnd = 0;
template<class T> int TextRenderer<T>::selectionXBegin = 0;
template<class T> int TextRenderer<T>::selectionXEnd = 0;
template<class T> int TextRenderer<T>::caretX = 0;

template<class T> TextRenderer<T>::TextRenderer(Item* parent, const TextStyle *style, const QString& text_) :
	T(parent, style), text(text_), editable(true)
{
}

template<class T> TextRenderer<T>::TextRenderer(Item* parent, Model::Node *node, const TextStyle *style) :
	T(parent, node, style), editable(true)
{;
}

template<class T> const TextStyle* TextRenderer<T>::style() const
{
	return static_cast<const TextStyle*> (T::style());
}

template<class T> void TextRenderer<T>::setStyle(const ItemStyle* style)
{
	const TextStyle* s = dynamic_cast<const TextStyle*> (style);
	if ( !s ) throw VisualizationException("Invalid style type when calling TextRenderer::setStyle");
	T::setStyle(s);
}

template<class T> void TextRenderer<T>::setText(const QString& newText)
{
	text = newText;
}

template<class T> QString TextRenderer<T>::getText()
{
	return text;
}

template<class T> QString TextRenderer<T>::getSelectedText()
{
	if (selected == this)
	{
		int xstart = selectionBegin;
		int xend = selectionEnd;
		if ( xstart > xend )
		{
			xstart = selectionEnd;
			xend = selectionBegin;
		}

		return text.mid(xstart, xend - xstart);
	}
	else return QString();
}

template<class T> bool TextRenderer<T>::isEditable()
{
	return editable;
}

template<class T> void TextRenderer<T>::setEditable(bool editable_)
{
	editable = editable_;
}

template<class T> void TextRenderer<T>::determineChildren()
{
}

template<class T> void TextRenderer<T>::updateGeometry(int, int)
{
	this->setFlag(QGraphicsItem::ItemHasNoContents, false);
	QFontMetrics qfm(style()->font());

	QRectF bound;
	if (text.isEmpty()) bound.setRect(0, 0, MIN_TEXT_WIDTH, qfm.height());
	else
	{
		bound = qfm.boundingRect(text);
		if (bound.width() < qfm.width(text)) bound.setWidth(qfm.width(text));
		if (bound.height() < qfm.height()) bound.setHeight(qfm.height());
	}

	xOffset = -bound.left();
	yOffset = -bound.top();
	this->setSize(bound.size());

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

template<class T> void TextRenderer<T>::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if ( selected != this )
	{
		painter->setPen(style()->pen());
		painter->setFont(style()->font());
		painter->drawText(xOffset, yOffset, text);
	}
	else
	{
		if ( selectionXBegin == selectionXEnd )
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

			// Draw selection background
			painter->setPen(Qt::NoPen);
			painter->setBrush(style()->selectionBackground());
			painter->drawRect(xOffset + selectionXBegin, 0, selectionXEnd - selectionXBegin, this->height());
			painter->setBrush(Qt::NoBrush);

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
		painter->drawLine(xOffset + caretX, 1, xOffset + caretX, this->height() - 1);
	}
}

template<class T> void TextRenderer<T>::selectAll()
{
	selectionBegin = 0;
	selectionEnd = text.length();
	selected = this;
	this->setUpdateNeeded();
}

template<class T> void TextRenderer<T>::setSelectedCharacters(int first, int last)
{
	selectionBegin = first;
	selectionEnd = last;
	selected = this;
	this->setUpdateNeeded();
}

template<class T> void TextRenderer<T>::setSelectedByDrag(int xBegin, int xEnd)
{
	selectionBegin = 0;
	selectionEnd = 0;

	QFontMetrics qfm(style()->font());
	int width = 0;
	for (int i = 1; i <= text.length(); ++i)
	{
		int new_width = qfm.width(text, i);
		if ( xBegin > (new_width + width + 1) / 2 ) selectionBegin++;
		if ( xEnd > (new_width + width + 1) / 2 ) selectionEnd++;
		width = new_width;
	}

	selected = this;
	this->setUpdateNeeded();
}

template<class T> void TextRenderer<T>::resetSelected()
{
	if ( selected ) selected->setUpdateNeeded();
	selected = NULL;
}

template<class T> int TextRenderer<T>::selectionFirstInxed()
{
	return selectionBegin < selectionEnd ? selectionBegin : selectionEnd;
}

template<class T> int TextRenderer<T>::selectionLastIndex()
{
	return selectionBegin > selectionEnd ? selectionBegin : selectionEnd;
}

template<class T> int TextRenderer<T>::caretPosition()
{
	return selectionEnd;
}

template<class T> void TextRenderer<T>::setCaretPosition(int beforeCharacter)
{
	setSelectedCharacters(beforeCharacter,beforeCharacter);
}

// Specialization of constructors
template<> TextRenderer<ModelItem>::TextRenderer(Item* parent, const TextStyle *style, const QString& text_) :
	ModelItem(parent, NULL, style), text(text_)
{
	throw VisualizationException("Creating a TextRenderer<ModelItem> by calling an inappropriate constructor");
}

template<> TextRenderer<Item>::TextRenderer(Item* parent, Model::Node *, const TextStyle *style) :
	Item(parent, style)
{
	throw VisualizationException("Creating a TextRenderer<Item> by calling an inappropriate constructor");
}

// Explicit instantiations
template class TextRenderer<Item> ;
template class TextRenderer<ModelItem> ;

}
