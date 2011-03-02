/***********************************************************************************************************************
 * Diamond.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Diamond.h"
#include "VisualizationException.h"

#include <QtGui/QPainter>

#include <cmath>

namespace Visualization {

Diamond::Diamond(Item *parent, StyleType *style) :
	Shape(parent, style)
{
}

void Diamond::setStyle(ShapeStyle *style)
{
	setGenericStyle<DiamondStyle> (style);
}

Diamond::StyleType* Diamond::style() const
{
	return static_cast<StyleType*> (Shape::style());
}

QPainterPath Diamond::getDiamondPath(qreal x, qreal y, int width, int height)
{
	QPainterPath path;

	path.moveTo(width/2 + x, y);
	path.lineTo(x, y + height/2);
	path.lineTo(x + width/2, y + width);
	path.lineTo(x + width, y + width/2);
	path.closeSubpath();
	return path;
}

void Diamond::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		contentTop_ = width()/2;
		contentLeft_ = height()/2;

		widthWithoutShadow = width()+height() + style()->outline().width();
		heightWithoutShadow = widthWithoutShadow;
	}
	else
	{
		widthWithoutShadow = width();
		heightWithoutShadow = height();

		contentTop_ = 0;
		contentLeft_ = 0;

		if ( style()->shadow() != Qt::NoBrush )
		{
			widthWithoutShadow -= style()->xShadowOffset();
			heightWithoutShadow -= style()->yShadowOffset();
		}
	}

	if ( style()->shadow() == Qt::NoBrush )
	{
		setItemSize(xOffset() + std::ceil(widthWithoutShadow), yOffset() + std::ceil(heightWithoutShadow));
	}
	else
	{
		setItemSize(xOffset() + std::ceil(widthWithoutShadow) + style()->xShadowOffset(), yOffset() + std::ceil(heightWithoutShadow) + style()->yShadowOffset());
	}
}

int Diamond::contentLeft()
{
	return xOffset() + contentLeft_;
}

int Diamond::contentTop()
{
	return yOffset() + contentTop_;
}

QSize Diamond::innerSize(QSize outterSize) const
{
	QSize inner(outterSize);
	inner -= QSize(std::ceil(style()->outline().width()), std::ceil(style()->outline().width()));
	if ( style()->shadow() != Qt::NoBrush ) inner -= QSize(style()->xShadowOffset(), style()->yShadowOffset());
	return inner;
}

QSize Diamond::outterSize(QSize innerSize) const
{
	int side = innerSize.width() + innerSize.height() + std::ceil(style()->outline().width());
	QSize outter(side, side);
	if ( style()->shadow() != Qt::NoBrush ) outter += QSize(style()->xShadowOffset(), style()->yShadowOffset());
	return outter;
}

void Diamond::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

	qreal x = xOffset();
	qreal y = yOffset();

	qreal outlineWidth = style()->outline().width();
	// Move the figure when using antialiasing. The outline will start at a pixel boundary. This makes it sharper.
	if ( painter->testRenderHint(QPainter::Antialiasing) || painter->testRenderHint(QPainter::HighQualityAntialiasing) )
		if ( style()->outline().style() != Qt::NoPen)
		{
			qreal intPart;
			qreal fracPart = std::modf(outlineWidth / 2.0, &intPart);

			x += fracPart;
			y += fracPart;
		}

	// Draw shadow
	if ( style()->shadow() != Qt::NoBrush )
	{
		painter->setPen(Qt::NoPen);
		painter->setBrush(style()->shadow());
		painter->drawPath(getDiamondPath(xOffset() + style()->xShadowOffset(), yOffset() + style()->yShadowOffset(), widthWithoutShadow, heightWithoutShadow));
	}

	// Draw box.
	painter->setPen(style()->outline());

	// Set the brush and fix the gradient if needed.
	if ( style()->background().style() == Qt::LinearGradientPattern && style()->background().gradient()->coordinateMode() == QGradient::LogicalMode )
	{
		QLinearGradient g = *(static_cast<const QLinearGradient*> (style()->background().gradient()));
		g.setStart(x + g.start().x(), y + g.start().y());
		g.setFinalStop(x + g.finalStop().x(), y + g.finalStop().y());
		painter->setBrush(g);

	}
	else if ( style()->background().style()  == Qt::RadialGradientPattern && style()->background().gradient()->coordinateMode() == QGradient::LogicalMode )
	{
		QRadialGradient g = *(static_cast<const QRadialGradient*> (style()->background().gradient()));
		g.setCenter(x + g.center().x(), y + g.center().y());
		g.setFocalPoint(x + g.focalPoint().x(), y + g.focalPoint().y());
		painter->setBrush(g);
	}
	else
	{
		painter->setBrush(style()->background());;
	}

	painter->drawPath(getDiamondPath(x, y, widthWithoutShadow - outlineWidth, heightWithoutShadow - outlineWidth));
}

}
