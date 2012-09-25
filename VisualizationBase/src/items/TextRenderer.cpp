/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

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
#include "cursor/TextCursor.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(TextRenderer, "item")

TextRenderer::TextRenderer(Item* parent, const StyleType *style, const QString& text) :
	Item(parent, style), text_(text), editable(true)
{
}

bool TextRenderer::setText(const QString& newText)
{
	text_ = newText;
	this->setUpdateNeeded(StandardUpdate);
	return true;
}

QString TextRenderer::selectedText()
{
	if (this->hasFocus())
	{
		TextCursor* cur = correspondingSceneCursor<TextCursor>();
		int xstart = cur->selectionFirstIndex();
		int xend = cur->selectionLastIndex();

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
		textXOffset_ = -bound.left() + this->getShape()->contentLeft();
		textYOffset_ = -bound.top() + this->getShape()->contentTop();
	}
	else
	{
		textXOffset_ = -bound.left();
		textYOffset_ = -bound.top();
		this->setSize(bound.size());
	}

	// Correct underline, otherwise it is drawn in the middle of two pixels and appears fat and transparent.
	if (style()->font().underline() && qfm.lineWidth() % 2)
	{
		textXOffset_ += 0.5;
		textYOffset_ += 0.5;
	}

	if ( this->hasFocus() )	correspondingSceneCursor<TextCursor>()->update(qfm);
}

void TextRenderer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (isCategoryHiddenDuringPaint()) return;

	Item::paint(painter, option, widget);

	//int numSelected = this->scene()->selectedItems().size();

	if ( !this->hasFocus() /*|| numSelected > 1  || (numSelected == 1 && !this->isSelected())*/)
	{
		painter->setPen(style()->pen());
		painter->setFont(style()->font());
		painter->drawText(QPointF(textXOffset_, textYOffset_), text_);
	}
	else
	{
		TextCursor* cur = correspondingSceneCursor<TextCursor>();
		if ( cur->hasSelection() )
		{
			// Some text is selected, draw it differently than non-selected text.
			int start = cur->selectionFirstIndex();
			int end = cur->selectionLastIndex();

			// Draw selection background
			painter->setPen(Qt::NoPen);
			painter->setBrush(style()->selectionBackground());
			painter->drawRect(textXOffset_ + cur->xBegin(), 0, cur->xEnd() - cur->xBegin(), this->height());
			painter->setBrush(Qt::NoBrush);

			// Draw selected text
			painter->setPen(style()->selectionPen());
			painter->setFont(style()->selectionFont());
			painter->drawText(QPointF(textXOffset_ + cur->xBegin(), textYOffset_), text_.mid(start, end - start));

			// Draw non-selected text
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawText(textXOffset_, textYOffset_, text_.left(start));
			painter->drawText(QPointF(textXOffset_ + cur->xEnd(), textYOffset_), text_.mid(end));
		}
		else
		{
			// No text is selected, draw all text at once using normal style
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawText(QPointF(textXOffset_, textYOffset_), text_);
		}
	}
}

bool TextRenderer::moveCursor(CursorMoveDirection dir, QPoint reference)
{
	if ( dir == MoveUpOf || dir == MoveDownOf || dir == MoveLeftOf || dir == MoveRightOf )
	{
		PositionConstraints pc = satisfiedPositionConstraints(reference);
		if ( (dir == MoveUpOf && (pc & Above))
				|| (dir == MoveDownOf && (pc & Below))
				|| (dir == MoveLeftOf && (pc & LeftOf))
				|| (dir == MoveRightOf && (pc & RightOf))
				)
		{
			TextCursor* tc = new TextCursor(this);
			tc->setSelectedByDrag(reference.x(), reference.x());
			scene()->setMainCursor(tc);
			return true;
		}
		else return false;
	}
	else if (dir == MoveOnPosition || dir == MoveDefault)
	{
		setFocus();
		TextCursor* tc = new TextCursor(this);

		if (dir == MoveDefault) tc->setCaretPosition(0);
		else tc->setSelectedByDrag(reference.x(), reference.x());

		scene()->setMainCursor(tc);
		return true;
	}
	else if (dir == MoveLeft)
	{
		int position = correspondingSceneCursor<Visualization::TextCursor>()->caretPosition();
		if ( text_.isEmpty() || position <= 0) return false;
		else correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(position - 1);
		return true;
	}
	else if (dir == MoveRight)
	{
		int position = correspondingSceneCursor<Visualization::TextCursor>()->caretPosition();
		if ( text_.isEmpty() || position >= text_.size()) return false;
		else correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(position + 1);
		return true;
	}

	return false;
}

}
