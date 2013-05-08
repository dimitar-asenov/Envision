/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "items/TextRenderer.h"

#include "items/ItemWithNode.h"
#include "Scene.h"
#include "VisualizationException.h"
#include "shapes/Shape.h"
#include "cursor/TextCursor.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(TextRenderer, "item")

TextRenderer::TextRenderer(Item* parent, const StyleType *style, const QString& text) :
Super(parent, style), staticText_(text), editable(true)
{
	staticText_.setPerformanceHint(QStaticText::AggressiveCaching);
	staticText_.setTextFormat(Qt::PlainText);
}

bool TextRenderer::setText(const QString& newText)
{
	staticText_.setText( newText );
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

		return staticText_.text().mid(xstart, xend - xstart);
	}
	else return QString();
}

void TextRenderer::determineChildren()
{
}

inline QRectF TextRenderer::bound()
{
	QFontMetrics qfm(style()->font());
	return bound(qfm);
}

inline QRectF TextRenderer::bound(QFontMetrics& qfm)
{
	QRectF bound;

	if (staticText_.text().isEmpty()) bound.setRect(0, 0, MIN_TEXT_WIDTH, qfm.height());
	else
	{
		bound = qfm.boundingRect(staticText_.text());
		if (bound.width() < qfm.width(staticText_.text())) bound.setWidth(qfm.width(staticText_.text()));
		if (bound.height() < qfm.height()) bound.setHeight(qfm.height());
	}
	return bound;
}

void TextRenderer::updateGeometry(int, int)
{
	staticText_.setText(currentText());

	QFontMetrics qfm(style()->font());
	auto bound = this->bound(qfm);
	if (this->hasShape())
	{
		this->getShape()->setInnerSize(bound.width(), bound.height());
		textXOffset_ = this->getShape()->contentLeft();
		textYOffset_ = this->getShape()->contentTop();
	}
	else
	{
		textXOffset_ = 0;
		textYOffset_ = 0;
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
		// In this common case use static text.
		painter->setPen(style()->pen());
		painter->setFont(style()->font());
		painter->drawStaticText(QPointF(textXOffset_, textYOffset_), staticText_);
	}
	else
	{
		// In the selected case do not use static text.

		TextCursor* cur = correspondingSceneCursor<TextCursor>();
		if ( cur->hasSelection() )
		{
			// Some text is selected, draw it differently than non-selected text.
			int start = cur->selectionFirstIndex();
			int end = cur->selectionLastIndex();

			QPointF offset(textXOffset_, textYOffset_);
			offset -= bound().topLeft();

			// Draw selection background
			painter->setPen(Qt::NoPen);
			painter->setBrush(style()->selectionBackground());
			painter->drawRect(textXOffset_ + cur->xBegin(), 0, cur->xEnd() - cur->xBegin(), this->height());
			painter->setBrush(Qt::NoBrush);

			// Draw selected text
			painter->setPen(style()->selectionPen());
			painter->setFont(style()->selectionFont());
			painter->drawText(QPointF(offset.x() + cur->xBegin(), offset.y()),
					staticText_.text().mid(start, end - start));

			// Draw non-selected text
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawText(offset, staticText_.text().left(start));
			painter->drawText(QPointF(offset.x() + cur->xEnd(), offset.y()), staticText_.text().mid(end));
		}
		else
		{
			// No text is selected, draw all text at once using normal style
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawStaticText(QPointF(textXOffset_, textYOffset_), staticText_);
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
		if ( staticText_.text().isEmpty() || position <= 0) return false;
		else correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(position - 1);
		return true;
	}
	else if (dir == MoveRight)
	{
		int position = correspondingSceneCursor<Visualization::TextCursor>()->caretPosition();
		if ( staticText_.text().isEmpty() || position >= staticText_.text().size()) return false;
		else correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(position + 1);
		return true;
	}

	return false;
}

}
