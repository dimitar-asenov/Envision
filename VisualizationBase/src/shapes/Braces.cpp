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

#include "Braces.h"
#include "../VisualizationException.h"

namespace Visualization {

SHAPE_COMMON_DEFINITIONS(Braces, "shape")

// This is needed to work around an issue where very big brace sizes result in huge braces (much bigger than the size)
// overlaid on top of the canvas, covering big parts of the project.
const int Braces::MAX_BRACE_SIZE = 1000;

Braces::Braces(Item *parent, StyleType *style) :
	Super{parent, style}, contentTop_{0}, contentLeft_{0}, textSize_{2}
{
}

void Braces::update()
{
	QSize lb;
	QSize rb;

	if ( sizeSpecified() == InnerSize )
	{
		textSize_ = height();
		if (textSize_ < style()->minHeight()) textSize_ = style()->minHeight();
		lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
		rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);

		while (lb.height() < height() && rb.height() < height() && textSize_ <= MAX_BRACE_SIZE)
		{
			textSize_++;
			lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
			rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);
		}

		contentTop_ = (lb.height() - height() )/ 2;
		contentLeft_ = lb.width();
		rightBraceLeft_ = contentLeft_ + width();

		setItemSize(xOffset() + lb.width()+width()+rb.width(), yOffset() + lb.height());
	}
	else
	{
		textSize_ = height();
		if (textSize_ < style()->minHeight()) textSize_ = style()->minHeight();
		lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
		rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);

		while (textSize_ >= 0 && (lb.width() + rb.width() > width() || lb.height() + rb.height() > height()))
		{
			textSize_--;
			lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
			rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);
		}

		contentTop_ = 0;
		contentLeft_ = lb.width();
		rightBraceLeft_ = width() - rb.width();

		setItemSize(xOffset() + width(), yOffset() + height());
	}

	if (textSize_ > MAX_BRACE_SIZE)
	{
		textSize_ = MAX_BRACE_SIZE;
		//QPointF leftOffset;
		//QPointF rightOffset;
		auto adjustedLb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
		auto adjustedRb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);

		leftBraceOffset_.ry() += (lb.height() - adjustedLb.height()) / 2.0;
		leftBraceOffset_.rx() += (lb.width() - adjustedLb.width()) / 2.0;
		rightBraceOffset_.ry() += (rb.height() - adjustedRb.height()) / 2.0;
		rightBraceOffset_.rx() += (rb.width() - adjustedRb.width()) / 2.0;
	}
}

QSize Braces::getSizeOfBrace(const QString& brace, const QFont& font, int innerHeight, QPointF* offset) const
{
	if (innerHeight < 1) return QSize{0, 0};

	if (brace.isEmpty()) return QSize{0, innerHeight};

	QFont f(font);
	f.setPixelSize(innerHeight);

	// The call to tightBoundingRect below can be very slow (originally, supposedly only in Windows, but as of 2017
	// also in Linux). So we cache it.
	//
	// Usually this cache is rather small so we don't need to worry about cleaning it up.
	// Testing this with the entire jEdit code base generated just 116 entries.
	// If the size of the cache ever becomes a problem, it might be an option to limit its use just to smaller
	// items, which should be the vast majority.
	static QHash<QPair<QFont, QString>, QRect> cache;
	auto searchPair = qMakePair(f, brace);
	auto foundIt = cache.find(searchPair);

	QRect bound = foundIt != cache.end() ? (*foundIt) : QFontMetrics{f}.tightBoundingRect(brace);
	if (foundIt == cache.end())
		cache.insert(searchPair, bound);

	if (offset)
	{
		offset->setX(-bound.left());
		offset->setY(-bound.top());
	}

	return bound.size();
}

int Braces::contentLeft()
{
	return xOffset() + contentLeft_;
}

int Braces::contentTop()
{
	return yOffset() + contentTop_;
}

QRect Braces::contentRect()
{
	return QRectF{xOffset() + contentLeft_, yOffset() + contentTop_,
					 rightBraceLeft_ + rightBraceOffset_.x() - contentLeft_, (qreal) height() }.toRect();
}

QSize Braces::innerSize(QSize outerSize) const
{
	int size = outerSize.height();
	QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), size, nullptr);
	QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), size, nullptr);

	while (size >= 0 && (lb.width() + rb.width() > outerSize.width() || lb.height() + rb.height() > outerSize.height()))
	{
		size--;
		lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), size, nullptr);
		rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), size, nullptr);
	}

	return QSize{ outerSize.width() - lb.width() - rb.width(), outerSize.height()};
}

QSize Braces::outerSize(QSize innerSize) const
{
	int size = innerSize.height();
	QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), size, nullptr);
	QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), size, nullptr);

	return QSize{ lb.width() + innerSize.width() + rb.width(), lb.height()};
}

void Braces::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	// Draw left brace.
	painter->setPen(style()->leftBracePen());
	QFont fl{style()->leftBraceFont()};
	fl.setPixelSize(textSize_);
	painter->setFont(fl);
	painter->drawText(QPointF{(qreal) xOffset(), (qreal) yOffset()} + leftBraceOffset_, style()->leftBrace());

	// Draw right brace.
	painter->setPen(style()->rightBracePen());
	QFont fr{style()->rightBraceFont()};
	fr.setPixelSize(textSize_);
	painter->setFont(fr);
	painter->drawText(QPointF{xOffset()+rightBraceLeft_, (qreal) yOffset()} + rightBraceOffset_, style()->rightBrace());
}

}
