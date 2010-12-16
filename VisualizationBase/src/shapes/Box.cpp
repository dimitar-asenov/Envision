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
	else
		path.arcTo(width - 2 * radius + x, y, radius * 2, radius * 2, 0.0, 90.0);

	path.lineTo(radius + x, y);

	if ( corner == BoxStyle::RightAngleCorner ) path.lineTo(x, y);
	else if ( corner == BoxStyle::CutCorner ) path.lineTo(x, radius + y);
	else
		path.arcTo(x, y, radius * 2, radius * 2, 90.0, 90.0);

	path.lineTo(x, height - radius + y);

	if ( corner == BoxStyle::RightAngleCorner ) path.lineTo(x, height + y);
	else if ( corner == BoxStyle::CutCorner ) path.lineTo(x + radius, height + y);
	else
		path.arcTo(x, height - 2 * radius + y, radius * 2, radius * 2, 180.0, 90.0);

	path.lineTo(width - radius + x, height + y);

	if ( corner == BoxStyle::RightAngleCorner ) path.lineTo(width + x, height + y);
	else if ( corner == BoxStyle::CutCorner ) path.lineTo(x + width, height - radius + y);
	else
		path.arcTo(width - 2 * radius + x, height - 2 * radius + y, radius * 2, radius * 2, 270.0, 90.0);

	path.closeSubpath();
	return path;
}

void Box::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		contentBoxWidth = width() + 2*style()->cornerRadius() + style()->outline().width();
		contentBoxHeight = height() + 2*style()->cornerRadius() + style()->outline().width();
	}
	else
	{
		contentBoxWidth = width();
		contentBoxHeight = height();

		if (style()->shadow() != Qt::NoBrush && style()->isShadowPartOfSize())
		{
			contentBoxWidth -= style()->xShadowOffset();
			contentBoxHeight -= style()->yShadowOffset();
		}
	}

	if (style()->shadow() == Qt::NoBrush || !style()->isShadowPartOfSize())
		setItemSize(std::ceil(contentBoxWidth), std::ceil(contentBoxHeight));
	else
		setItemSize(std::ceil(contentBoxWidth) + style()->xShadowOffset(), std::ceil(contentBoxHeight) + style()->yShadowOffset());

	if (style()->shadow() == Qt::NoBrush)
		setItemBoundingRect(xOffset(), yOffset(), std::ceil(contentBoxWidth), std::ceil(contentBoxHeight));
	else
		setItemBoundingRect(xOffset(), yOffset(), std::ceil(contentBoxWidth) + style()->xShadowOffset(), std::ceil(contentBoxHeight) + style()->yShadowOffset());
}

int Box::contentLeft()
{
	return xOffset() + style()->cornerRadius() + (style()->outline().width()+1)/2;
}

int Box::contentTop()
{
	return yOffset() + style()->cornerRadius() + (style()->outline().width()+1)/2;
}

int Box::getOutterWidth(int innerWidth) const
{
	int outterWidth = innerWidth + 2*style()->cornerRadius() + std::ceil(style()->outline().width());
	if (style()->shadow() != Qt::NoBrush && style()->isShadowPartOfSize()) outterWidth += style()->xShadowOffset();
	return outterWidth;
}

int Box::getOutterHeight(int innerHeight) const
{
	int outterHeight = innerHeight + 2*style()->cornerRadius() + std::ceil(style()->outline().width());
	if (style()->shadow() != Qt::NoBrush && style()->isShadowPartOfSize()) outterHeight += style()->yShadowOffset();
	return outterHeight;
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

	qreal x = xOffset();
	qreal y = yOffset();

	qreal outlineWidth = style()->outline().width();
	// Move the figure when using antialiasing. The outline will start at a pixel boundary. This makes it sharper.
	if (painter->testRenderHint(QPainter::Antialiasing) ||  painter->testRenderHint(QPainter::HighQualityAntialiasing) )
		if (style()->outline() != Qt::NoPen )
		{
			qreal intPart;
			qreal fracPart = std::modf( outlineWidth / 2.0, &intPart );

			x += fracPart;
			y += fracPart;
		}

	// Draw shadow
	if ( style()->shadow() != Qt::NoBrush )
	{
		painter->setPen(Qt::NoPen);
		painter->setBrush(style()->shadow());
		painter->drawPath(getRectanglePath(x + style()->xShadowOffset(), y + style()->yShadowOffset(), contentBoxWidth, contentBoxHeight));
	}

	// Draw box.
	painter->setPen(style()->outline());
	painter->setBrush(style()->background());
	painter->drawPath(getRectanglePath(x, y, contentBoxWidth - outlineWidth , contentBoxHeight - outlineWidth));
}

}
