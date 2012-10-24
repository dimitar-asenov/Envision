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
 * Box.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Box.h"
#include "VisualizationException.h"

namespace Visualization {

SHAPE_COMMON_DEFINITIONS(Box, "shape")

Box::Box(Item *parent, StyleType *style) :
	Shape(parent, style)
{
}

QPainterPath Box::getRectanglePath(qreal x, qreal y, int width, int height)
{
	QPainterPath path;
	int radius = style()->cornerRadius();
	if (radius > width/2) radius = width/2;
	if (radius > height/2) radius = height/2;
	BoxStyle::CornerType corner = style()->corner();

	path.moveTo(width + x, radius + y);

	if ( corner == BoxStyle::CornerType::RightAngle ) path.lineTo(width + x, y);
	else if ( corner == BoxStyle::CornerType::Cut ) path.lineTo(width + x - radius, y);
	else path.arcTo(width - 2 * radius + x, y, radius * 2, radius * 2, 0.0, 90.0);

	path.lineTo(radius + x, y);

	if ( corner == BoxStyle::CornerType::RightAngle ) path.lineTo(x, y);
	else if ( corner == BoxStyle::CornerType::Cut ) path.lineTo(x, radius + y);
	else path.arcTo(x, y, radius * 2, radius * 2, 90.0, 90.0);

	path.lineTo(x, height - radius + y);

	if ( corner == BoxStyle::CornerType::RightAngle ) path.lineTo(x, height + y);
	else if ( corner == BoxStyle::CornerType::Cut ) path.lineTo(x + radius, height + y);
	else path.arcTo(x, height - 2 * radius + y, radius * 2, radius * 2, 180.0, 90.0);

	path.lineTo(width - radius + x, height + y);

	if ( corner == BoxStyle::CornerType::RightAngle ) path.lineTo(width + x, height + y);
	else if ( corner == BoxStyle::CornerType::Cut ) path.lineTo(x + width, height - radius + y);
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
		setItemSize(xOffset() + std::ceil(contentBoxWidth) + style()->xShadowOffset(),
				yOffset() + std::ceil(contentBoxHeight) + style()->yShadowOffset());
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

QSize Box::innerSize(QSize outterSize) const
{
	int innerWidth = outterSize.width() - 2 * style()->cornerRadius() - std::ceil(style()->outline().width());
	int innerHeight = outterSize.height() - 2 * style()->cornerRadius() - std::ceil(style()->outline().width());

	if ( style()->shadow() != Qt::NoBrush )
	{
		innerWidth -= style()->xShadowOffset();
		innerHeight -= style()->yShadowOffset();
	}

	return QSize(innerWidth, innerHeight);
}

QSize Box::outterSize(QSize innerSize) const
{
	int outterWidth = innerSize.width() + 2 * style()->cornerRadius() + std::ceil(style()->outline().width());
	int outterHeight = innerSize.height() + 2 * style()->cornerRadius() + std::ceil(style()->outline().width());

	if ( style()->shadow() != Qt::NoBrush )
	{
		outterWidth += style()->xShadowOffset();
		outterHeight += style()->yShadowOffset();
	}

	return QSize(outterWidth, outterHeight);
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
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
		painter->drawPath(getRectanglePath(xOffset() + style()->xShadowOffset(), yOffset() + style()->yShadowOffset(),
				contentBoxWidth, contentBoxHeight));
	}

	// Draw box.
	painter->setPen(style()->outline());

	// Set the brush and fix the gradient if needed.
	if ( style()->background().style() == Qt::LinearGradientPattern
			&& style()->background().gradient()->coordinateMode() == QGradient::LogicalMode )
	{
		QLinearGradient g = *(static_cast<const QLinearGradient*> (style()->background().gradient()));
		g.setStart(x + g.start().x(), y + g.start().y());
		g.setFinalStop(x + g.finalStop().x(), y + g.finalStop().y());
		painter->setBrush(g);

	}
	else if ( style()->background().style()  == Qt::RadialGradientPattern
			&& style()->background().gradient()->coordinateMode() == QGradient::LogicalMode )
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
