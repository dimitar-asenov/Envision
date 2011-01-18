/***********************************************************************************************************************
 * Box.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Box.h"
#include "VisualizationException.h"

#include <QtGui/QPainter>

#include <cmath>

namespace Visualization {

Box::Box(Item *parent, BoxStyle *style) :
	Shape(parent, style)
{
}

void Box::setStyle(ShapeStyle *style)
{
	setGenericStyle<BoxStyle> (style);
}

BoxStyle* Box::style() const
{
	return static_cast<BoxStyle*> (Shape::style());
}

QPainterPath Box::getRectanglePath(qreal x, qreal y, int width, int height)
{
	QPainterPath path;
	int radius = style()->cornerRadius();
	BoxStyle::CornerType corner = style()->corner();

	path.moveTo(width + x, radius + y);

	if ( corner == BoxStyle::RightAngleCorner ) path.lineTo(width + x, y);
	else if ( corner == BoxStyle::CutCorner ) path.lineTo(width + x - radius, y);
	else path.arcTo(width - 2 * radius + x, y, radius * 2, radius * 2, 0.0, 90.0);

	path.lineTo(radius + x, y);

	if ( corner == BoxStyle::RightAngleCorner ) path.lineTo(x, y);
	else if ( corner == BoxStyle::CutCorner ) path.lineTo(x, radius + y);
	else path.arcTo(x, y, radius * 2, radius * 2, 90.0, 90.0);

	path.lineTo(x, height - radius + y);

	if ( corner == BoxStyle::RightAngleCorner ) path.lineTo(x, height + y);
	else if ( corner == BoxStyle::CutCorner ) path.lineTo(x + radius, height + y);
	else path.arcTo(x, height - 2 * radius + y, radius * 2, radius * 2, 180.0, 90.0);

	path.lineTo(width - radius + x, height + y);

	if ( corner == BoxStyle::RightAngleCorner ) path.lineTo(width + x, height + y);
	else if ( corner == BoxStyle::CutCorner ) path.lineTo(x + width, height - radius + y);
	else path.arcTo(width - 2 * radius + x, height - 2 * radius + y, radius * 2, radius * 2, 270.0, 90.0);

	path.closeSubpath();
	return path;
}

void Box::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		contentBoxWidth = width() + 2 * style()->cornerRadius() + style()->outline().width();
		contentBoxHeight = height() + 2 * style()->cornerRadius() + style()->outline().width();
	}
	else
	{
		contentBoxWidth = width();
		contentBoxHeight = height();

		if ( style()->shadow() != Qt::NoBrush )
		{
			contentBoxWidth -= style()->xShadowOffset();
			contentBoxHeight -= style()->yShadowOffset();
		}
	}

	if ( style()->shadow() == Qt::NoBrush )
	{
		setItemSize(xOffset() + std::ceil(contentBoxWidth), yOffset() + std::ceil(contentBoxHeight));
	}
	else
	{
		setItemSize(xOffset() + std::ceil(contentBoxWidth) + style()->xShadowOffset(), yOffset() + std::ceil(contentBoxHeight) + style()->yShadowOffset());
	}
}

int Box::contentLeft()
{
	return xOffset() + style()->cornerRadius() + (style()->outline().width() + 1) / 2;
}

int Box::contentTop()
{
	return yOffset() + style()->cornerRadius() + (style()->outline().width() + 1) / 2;
}

int Box::getInnerWidth(int outterWidth) const
{
	int innerWidth = outterWidth - 2 * style()->cornerRadius() - std::ceil(style()->outline().width());
	if ( style()->shadow() != Qt::NoBrush ) innerWidth -= style()->xShadowOffset();
	return innerWidth;
}

int Box::getInnerHeight(int outterHeight) const
{
	int innerHeight = outterHeight - 2 * style()->cornerRadius() - std::ceil(style()->outline().width());
	if ( style()->shadow() != Qt::NoBrush ) innerHeight -= style()->yShadowOffset();
	return innerHeight;
}

int Box::getOutterWidth(int innerWidth) const
{
	int outterWidth = innerWidth + 2 * style()->cornerRadius() + std::ceil(style()->outline().width());
	if ( style()->shadow() != Qt::NoBrush ) outterWidth += style()->xShadowOffset();
	return outterWidth;
}

int Box::getOutterHeight(int innerHeight) const
{
	int outterHeight = innerHeight + 2 * style()->cornerRadius() + std::ceil(style()->outline().width());
	if ( style()->shadow() != Qt::NoBrush ) outterHeight += style()->yShadowOffset();
	return outterHeight;
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

	qreal x = xOffset();
	qreal y = yOffset();

	qreal outlineWidth = style()->outline().width();
	// Move the figure when using antialiasing. The outline will start at a pixel boundary. This makes it sharper.
	if ( painter->testRenderHint(QPainter::Antialiasing) || painter->testRenderHint(QPainter::HighQualityAntialiasing) ) if ( style()->outline()
			!= Qt::NoPen )
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
		painter->drawPath(getRectanglePath(xOffset() + style()->xShadowOffset(), yOffset() + style()->yShadowOffset(), contentBoxWidth, contentBoxHeight));
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

	painter->drawPath(getRectanglePath(x, y, contentBoxWidth - outlineWidth, contentBoxHeight - outlineWidth));
}

}
