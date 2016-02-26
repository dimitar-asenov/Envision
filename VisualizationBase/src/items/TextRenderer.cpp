/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "TextRenderer.h"

#include "ItemWithNode.h"
#include "../Scene.h"
#include "../VisualizationException.h"
#include "../shapes/Shape.h"
#include "../cursor/TextCursor.h"

namespace Visualization {

DEFINE_ITEM_COMMON(TextRenderer, "item")

TextRenderer::TextRenderer(Item* parent, const StyleType *style, const QString& text) :
Super{parent, style}, staticText_{text}, editable_(true)
{
	staticText_.setPerformanceHint(QStaticText::AggressiveCaching);
	setTextFormat(style->htmlFormat() ? Qt::RichText : Qt::PlainText);
}

bool TextRenderer::setText(const QString& newText)
{
	staticText_.setText( newText );
	this->setUpdateNeeded(StandardUpdate);
	return true;
}

QString TextRenderer::selectedText()
{
	if (hasSceneCursor() && !isHtml())
	{
		TextCursor* cur = correspondingSceneCursor<TextCursor>();
		int xstart = cur->selectionFirstIndex();
		int xend = cur->selectionLastIndex();

		return staticText_.text().mid(xstart, xend - xstart);
	}
	else return QString{};
}

void TextRenderer::determineChildren()
{
}

inline QSize TextRenderer::staticTextSize(QFontMetrics& qfm)
{
	if (staticText_.text().isEmpty()) return QSize{MIN_TEXT_WIDTH, qfm.height()};
	else return staticText_.size().toSize();
}

inline QRectF TextRenderer::nonStaticTextBound()
{
	Q_ASSERT(!isHtml());

	QRectF bound;
	QFontMetrics qfm{style()->font()};
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
	QFontMetrics qfm{style()->font()};
	QSize textSize;

	if (drawApproximately_ == Unknown)
		drawApproximately_ = scene()->approximateUpdate() ? Approximate : Exact;

	if (drawApproximately_ == Approximate)
	{
		int approxWidth = currentText().length() * qfm.averageCharWidth();
		textSize = QSize{approxWidth, qfm.height()};
	}
	else
	{
		// It's important to call this in order to give the static text the correct font. This is used when computing bound.
		staticText_.prepare(QTransform{}, style()->font());
		textSize = this->staticTextSize(qfm);
	}

	if (this->hasShape())
	{
		this->getShape()->setInnerSize(textSize.width(), textSize.height());
		textXOffset_ = this->getShape()->contentLeft();
		textYOffset_ = this->getShape()->contentTop();
	}
	else
	{
		textXOffset_ = 0;
		textYOffset_ = 0;
		this->setSize(textSize);
	}

	// Correct underline, otherwise it is drawn in the middle of two pixels and appears fat and transparent.
	// This was updated when drawing the line manually (in order to workaround qt no translating the line properly when
	// scaling). The old condition is below
	// if (style()->font().underline() && (qfm.lineWidth() % 2 == 0))
	if (style()->underline() && (qfm.lineWidth() % 2 == 1))
	{
		textXOffset_ += 0.5;
		textYOffset_ += 0.5;
	}

	if ( hasSceneCursor() && !isHtml())
		correspondingSceneCursor<TextCursor>()->update(qfm);
}

void TextRenderer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (drawApproximately_ == Approximate)
	{
		setUpdateNeeded(StandardUpdate);
		drawApproximately_ = Exact;
		return;
	}
	if (isCategoryHiddenDuringPaint()) return;

	Item::paint(painter, option, widget);

	TextCursor* selectionCursor = (hasSceneCursor() && !isHtml()) ? correspondingSceneCursor<TextCursor>() : nullptr;
	if (selectionCursor && !selectionCursor->hasSelection()) selectionCursor = nullptr;

	if ( !selectionCursor )
	{
		// In this common case use static text.
		qreal size = painter->worldTransform().m11() * style()->font().pixelSize();
		if (size < 0.5) return;

		if (size < 3.0)
		{
			// Draw just a filler
			auto c = style()->pen().color();
			c.setAlpha(64);
			painter->fillRect(QRectF{textXOffset_, textYOffset_ + heightInLocal()/4.0,
											 staticText_.size().width(), staticText_.size().height()/2.0}, c);
		}
		else
		{
			// Draw actual text
			painter->setPen(style()->pen());
			painter->setFont(style()->font());
			painter->drawStaticText(QPointF{textXOffset_, textYOffset_}, staticText_);
			if (style()->underline())
				painter->drawLine(QPointF{textXOffset_, textYOffset_ + staticText_.size().height()},
					QPointF{textXOffset_ + staticText_.size().width(), textYOffset_ + staticText_.size().height()});
		}
	}
	else
	{
		// In the selected case do not use static text.

		// Some text is selected, draw it differently than non-selected text.
		int start = selectionCursor->selectionFirstIndex();
		int end = selectionCursor->selectionLastIndex();

		QPointF offset{textXOffset_, textYOffset_};

		// Might be a bit slow but that's OK, since we have very little selected text
		// Here topLeft is the absolute corner of the text if it is drawn at 0, 0.
		offset -= nonStaticTextBound().topLeft();

		// Draw selection background
		painter->setPen(Qt::NoPen);
		painter->setBrush(style()->selectionBackground());
		painter->drawRect(textXOffset_ + selectionCursor->xBegin(), 0,
				selectionCursor->xEnd() - selectionCursor->xBegin(), this->heightInLocal());
		painter->setBrush(Qt::NoBrush);

		// Draw selected text
		painter->setPen(style()->selectionPen());
		painter->setFont(style()->selectionFont());
		painter->drawText(QPointF{offset.x() + selectionCursor->xBegin(), offset.y()},
				staticText_.text().mid(start, end - start));

		auto underlineY = textYOffset_ + staticText_.size().height();
		if (style()->underline())
			painter->drawLine(QPointF{offset.x() + selectionCursor->xBegin(), underlineY},
				QPointF{offset.x() + selectionCursor->xEnd(), underlineY});

		// Draw non-selected text
		painter->setPen(style()->pen());
		painter->setFont(style()->font());
		painter->drawText(offset, staticText_.text().left(start));
		painter->drawText(QPointF{offset.x() + selectionCursor->xEnd(), offset.y()}, staticText_.text().mid(end));

		if (style()->underline())
		{
			painter->drawLine(QPointF{textXOffset_, underlineY},
				QPointF{offset.x() + selectionCursor->xBegin(), underlineY});
			painter->drawLine(QPointF{offset.x() + selectionCursor->xEnd(), underlineY},
				QPointF{textXOffset_ + staticText_.size().width(), underlineY});
		}
	}
}

bool TextRenderer::moveCursor(CursorMoveDirection dir, QPoint reference, CursorMoveOptions options)
{
	if ( isHtml() ) return Super::moveCursor(dir, reference, options);

	if ( dir == MoveUpOf || dir == MoveDownOf || dir == MoveLeftOf || dir == MoveRightOf )
	{
		PositionConstraints pc = satisfiedPositionConstraints(reference);
		if ( (dir == MoveUpOf && (pc & Above))
				|| (dir == MoveDownOf && (pc & Below))
				|| (dir == MoveLeftOf && (pc & LeftOf))
				|| (dir == MoveRightOf && (pc & RightOf))
				)
		{
			TextCursor* tc = new TextCursor{this};
			tc->setSelectedByDrag(reference.x(), reference.x());
			scene()->setMainCursor(tc);
			return true;
		}
		else return false;
	}
	else if (dir == MoveOnPosition || dir == MoveDefault || dir == MoveOnTop || dir == MoveOnLeft || dir == MoveOnBottom
			|| dir == MoveOnRight || dir == MoveOnTopLeft || dir == MoveOnBottomRight || dir == MoveOnCenter)
	{
		setFocus();
		TextCursor* tc = new TextCursor{this};

		auto xEnd = widthInLocal() - 1;
		auto xMid = widthInLocal()/2;
		switch (dir)
		{
			case MoveDefault: tc->setCaretPosition(0); break;
			case MoveOnPosition: tc->setSelectedByDrag(reference.x(), reference.x()); break;
			case MoveOnTop: tc->setSelectedByDrag(xMid, xMid); break;
			case MoveOnLeft: tc->setSelectedByDrag(0, 0);  break;
			case MoveOnBottom: tc->setSelectedByDrag(xMid, xMid); break;
			case MoveOnRight: tc->setSelectedByDrag(xEnd, xEnd); break;
			case MoveOnTopLeft: tc->setSelectedByDrag(0, 0); break;
			case MoveOnBottomRight: tc->setSelectedByDrag(xEnd, xEnd); break;
			case MoveOnCenter: tc->setSelectedByDrag(xMid, xMid); break;
			default: Q_ASSERT(false); break;
		}

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

void TextRenderer::setTextFormat(Qt::TextFormat textFormat)
{
	Q_ASSERT(textFormat == Qt::PlainText || textFormat == Qt::RichText);

	if (textFormat != staticText_.textFormat())
	{
		staticText_.setTextFormat(textFormat);

		auto to = staticText_.textOption();
		to.setWrapMode(QTextOption::NoWrap);
		staticText_.setTextOption(to);

		this->setUpdateNeeded(StandardUpdate);
	}
}

bool TextRenderer::ignoresCopyAndPaste()
{
	return suppressDefaultCopyPasteHandler_;
}

}
