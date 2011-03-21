/***********************************************************************************************************************
 * Braces.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Braces.h"
#include "VisualizationException.h"

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

namespace Visualization {

Braces::Braces(Item *parent, StyleType *style) :
	Shape(parent, style), contentTop_(0), contentLeft_(0), textSize_(2)
{
}

void Braces::setStyle(ShapeStyle *style)
{
	setGenericStyle<BracesStyle> (style);
}

BracesStyle* Braces::style() const
{
	return static_cast<BracesStyle*> (Shape::style());
}

void Braces::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		textSize_ = height();
		QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
		QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);
		contentTop_ = (lb.height() - height() )/ 2;
		contentLeft_ = lb.width();
		rightBraceLeft_ = contentLeft_ + width();

		setItemSize(xOffset() + lb.width()+width()+rb.width(), yOffset() + lb.height());
	}
	else
	{
		textSize_ = height();
		QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), textSize_, &leftBraceOffset_);
		QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), textSize_, &rightBraceOffset_);

		while(textSize_ >= 0 && (lb.width() + rb.width() > width() || lb.height() + rb.height() > height()))
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
	if (innerHeight < 1) return QSize(0,0);

	if (brace.isEmpty()) return QSize(0, innerHeight);

	QFont f(font);
	f.setPixelSize(innerHeight);
	QFontMetrics qfm(f);

	QRect bound = qfm.boundingRect(brace);
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

QSize Braces::innerSize(QSize outterSize) const
{
	int size = outterSize.height();
	QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), size, NULL);
	QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), size, NULL);

	while(size >= 0 && (lb.width() + rb.width() > outterSize.width() || lb.height() + rb.height() > outterSize.height()))
	{
		size--;
		lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), size, NULL);
		rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), size, NULL);
	}

	return QSize( outterSize.width() - lb.width() - rb.width(), outterSize.height());
}

QSize Braces::outterSize(QSize innerSize) const
{
	int size = innerSize.height();
	QSize lb = getSizeOfBrace(style()->leftBrace(), style()->leftBraceFont(), size, NULL);
	QSize rb = getSizeOfBrace(style()->rightBrace(), style()->rightBraceFont(), size, NULL);

	return QSize( lb.width() + innerSize.width() + rb.width(), lb.height());
}

void Braces::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	// Draw left brace.
	painter->setPen(style()->leftBracePen());
	QFont fl(style()->leftBraceFont());
	fl.setPixelSize(textSize_);
	painter->setFont(fl);
	painter->drawText(QPointF(xOffset(), yOffset()) + leftBraceOffset_, style()->leftBrace());

	// Draw right brace.
	painter->setPen(style()->rightBracePen());
	QFont fr(style()->rightBraceFont());
	fr.setPixelSize(textSize_);
	painter->setFont(fr);
	painter->drawText(QPointF(xOffset()+rightBraceLeft_, yOffset()) + rightBraceOffset_, style()->rightBrace());
}

}
