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

#include "BoxStyle.h"

namespace Visualization {

BoxStyle::~BoxStyle(){}

QPainterPath BoxStyle::getRectanglePath(qreal x, qreal y, int width, int height) const
{
	QPainterPath path;
	int radius = cornerRadius();
	if (radius > width/2) radius = width/2;
	if (radius > height/2) radius = height/2;

	path.moveTo(width + x, radius + y);

	if ( corner() == CornerType::RightAngle ) path.lineTo(width + x, y);
	else if ( corner() == CornerType::Cut ) path.lineTo(width + x - radius, y);
	else path.arcTo(width - 2 * radius + x, y, radius * 2, radius * 2, 0.0, 90.0);

	path.lineTo(radius + x, y);

	if ( corner() == CornerType::RightAngle ) path.lineTo(x, y);
	else if ( corner() == CornerType::Cut ) path.lineTo(x, radius + y);
	else path.arcTo(x, y, radius * 2, radius * 2, 90.0, 90.0);

	path.lineTo(x, height - radius + y);

	if ( corner() == CornerType::RightAngle ) path.lineTo(x, height + y);
	else if ( corner() == CornerType::Cut ) path.lineTo(x + radius, height + y);
	else path.arcTo(x, height - 2 * radius + y, radius * 2, radius * 2, 180.0, 90.0);

	path.lineTo(width - radius + x, height + y);

	if ( corner() == CornerType::RightAngle ) path.lineTo(width + x, height + y);
	else if ( corner() == CornerType::Cut ) path.lineTo(x + width, height - radius + y);
	else path.arcTo(width - 2 * radius + x, height - 2 * radius + y, radius * 2, radius * 2, 270.0, 90.0);

	path.closeSubpath();
	return path;
}

void BoxStyle::paint(QPainter* painter, int xOffset, int yOffset, int contentBoxWidth, int contentBoxHeight,
							QColor customColor) const
{
	qreal scaleFactor = painter->worldTransform().m11();
	if (scaleFactor < 0.5 && scaleFactor*outline().width() < 0.1)
		simplifiedPaint(painter, xOffset, yOffset, contentBoxWidth, contentBoxHeight, customColor);
	else
		unoptimizedPaint(painter, xOffset, yOffset, contentBoxWidth, contentBoxHeight, customColor);
}

void BoxStyle::unoptimizedPaint(QPainter* painter, int xOffset, int yOffset, int contentBoxWidth,
				int contentBoxHeight, QColor customColor) const
{
	qreal x = xOffset;
	qreal y = yOffset;

	int outlineWidth = outline().style()!=Qt::NoPen ? outline().width() : 0;
	// Move the figure when using antialiasing. The outline will start at a pixel boundary. This makes it sharper.
	if ( painter->testRenderHint(QPainter::Antialiasing) )
		if ( outline().style() != Qt::NoPen)
		{
			x = xOffset + outlineWidth/2.0;
			y = yOffset + outlineWidth/2.0;
		}

	// Draw box.
	if (customColor.isValid())
	{
		auto pen = outline();
		pen.setColor(customColor.darker());
		painter->setPen(pen);
	}
	else
		painter->setPen(outline());

	// Set the brush and fix the gradient if needed.
	if (customColor.isValid())
		painter->setBrush(customColor);
	else if (background().style() == Qt::LinearGradientPattern || background().style()  == Qt::RadialGradientPattern)
		painter->setBrush(fixGradient(background(), x, y));
	else
		painter->setBrush(background());

	painter->drawPath(getRectanglePath(x, y, contentBoxWidth - outlineWidth, contentBoxHeight - outlineWidth));
}

void BoxStyle::simplifiedPaint(QPainter* painter, int xOffset, int yOffset, int contentBoxWidth,
						int contentBoxHeight, QColor customColor) const
{
	// Draw a simplified version
	if (background().style() != Qt::NoBrush)
	{
		if (background().gradient())
			painter->fillRect(xOffset, yOffset, contentBoxWidth, contentBoxHeight,
									background().gradient()->stops().last().second);
		else
			painter->fillRect(xOffset, yOffset, contentBoxWidth, contentBoxHeight,
									(customColor.isValid() ? customColor : background().color()));
	}
}

}
