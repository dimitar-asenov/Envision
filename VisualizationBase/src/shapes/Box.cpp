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
		contentBoxWidth = width() + 2*style()->cornerRadius() + std::ceil(style()->outline().width());
		contentBoxHeight = height() + 2*style()->cornerRadius() + std::ceil(style()->outline().width());
	}
	else
	{
		contentBoxWidth = width() - 2*style()->cornerRadius() + std::ceil(style()->outline().width());
		contentBoxHeight = height() - 2*style()->cornerRadius() + std::ceil(style()->outline().width());

		if (style()->shadow() != Qt::NoBrush && style()->isShadowPartOfSize())
		{
			contentBoxWidth -= style()->xShadowOffset();
			contentBoxHeight -= style()->yShadowOffset();
		}
	}

	if (style()->shadow() == Qt::NoBrush || !style()->isShadowPartOfSize())
		setItemSize(contentBoxWidth + xOffset(), contentBoxHeight + yOffset());
	else
		setItemSize(contentBoxWidth + style()->xShadowOffset() + xOffset(), contentBoxHeight + style()->yShadowOffset() + yOffset());

	if (style()->shadow() == Qt::NoBrush)
		setItemBoundingRect(xOffset(), yOffset(), contentBoxWidth, contentBoxHeight);
	else
		setItemBoundingRect(xOffset(), yOffset(), contentBoxWidth + style()->xShadowOffset(), contentBoxHeight + style()->yShadowOffset());
}

QPoint Box::contentPosition()
{
	return QPoint(xOffset() + style()->cornerRadius() + (style()->outline().width()+1)/2, yOffset() + style()->cornerRadius() + (style()->outline().width()+1)/2);
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	qreal outlineWidth = style()->outline().width();

	// Draw shadow
	if ( style()->shadow() != Qt::NoBrush )
	{
		painter->setPen(Qt::NoPen);
		painter->setBrush(style()->shadow());
		painter->drawPath(getRectanglePath(xOffset() + style()->xShadowOffset(), yOffset() + style()->yShadowOffset(), contentBoxWidth, contentBoxHeight));
	}

	// Draw box.
	painter->setPen(style()->outline());
	painter->setBrush(style()->background());

	qreal x = xOffset();
	qreal y = yOffset();

	// Move the figure when using antialiasing. The outline will start at a pixel boundary. This makes it sharper.
	if (painter->testRenderHint(QPainter::Antialiasing) ||  painter->testRenderHint(QPainter::HighQualityAntialiasing) )
		if (style()->outline() != Qt::NoPen )
		{
			qreal intPart;
			qreal fracPart = std::modf( outlineWidth / 2.0, &intPart );

			x += fracPart;
			y += fracPart;
		}

	painter->drawPath(getRectanglePath(x, y, contentBoxWidth - outlineWidth , contentBoxHeight - outlineWidth));
}

}
