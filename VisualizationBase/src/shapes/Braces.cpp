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

Braces::Braces(Item *parent, StyleType *style) :
	Super{parent, style}, contentTop_{0}, contentLeft_{0}, textSize_{2}
{
}

void Braces::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		textSize_ = height();
		if (textSize_ < style()->minHeight()) textSize_ = style()->minHeight();
		QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
		QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);

		while (lb.height() < height() && rb.height() < height())
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
		QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
		QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);

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
}

QSize Braces::getSizeOfBrace(const QString& brace, const QFont& font, int innerHeight, QPointF* offset) const
{
	if (innerHeight < 1) return QSize{0, 0};

	if (brace.isEmpty()) return QSize{0, innerHeight};

	QFont f(font);
	f.setPixelSize(innerHeight);
	QFontMetrics qfm{f};

	// TODO tightBoundingRect is supposedly very slow on Windows. Test this.
	QRect bound = qfm.tightBoundingRect(brace);
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
