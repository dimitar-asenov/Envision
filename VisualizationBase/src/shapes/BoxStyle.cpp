/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "shapes/BoxStyle.h"

namespace Visualization {

void BoxStyle::load(StyleLoader& sl)
{
	ShapeStyle::load(sl);
	sl.load("backgroundBrush", background_);
	// Indirect reading of enums in order to avoid compiler errors
	int enumVal{};
	sl.load("cornerType", enumVal);
	corner_ = (CornerType) enumVal;
	sl.load("cornerRadius", cornerRadius_);
}

QPainterPath BoxStyle::getRectanglePath(qreal x, qreal y, int width, int height) const
{
	QPainterPath path;
	int radius = cornerRadius_;
	if (radius > width/2) radius = width/2;
	if (radius > height/2) radius = height/2;

	path.moveTo(width + x, radius + y);

	if ( corner_ == CornerType::RightAngle ) path.lineTo(width + x, y);
	else if ( corner_ == CornerType::Cut ) path.lineTo(width + x - radius, y);
	else path.arcTo(width - 2 * radius + x, y, radius * 2, radius * 2, 0.0, 90.0);

	path.lineTo(radius + x, y);

	if ( corner_ == CornerType::RightAngle ) path.lineTo(x, y);
	else if ( corner_ == CornerType::Cut ) path.lineTo(x, radius + y);
	else path.arcTo(x, y, radius * 2, radius * 2, 90.0, 90.0);

	path.lineTo(x, height - radius + y);

	if ( corner_ == CornerType::RightAngle ) path.lineTo(x, height + y);
	else if ( corner_ == CornerType::Cut ) path.lineTo(x + radius, height + y);
	else path.arcTo(x, height - 2 * radius + y, radius * 2, radius * 2, 180.0, 90.0);

	path.lineTo(width - radius + x, height + y);

	if ( corner_ == CornerType::RightAngle ) path.lineTo(width + x, height + y);
	else if ( corner_ == CornerType::Cut ) path.lineTo(x + width, height - radius + y);
	else path.arcTo(width - 2 * radius + x, height - 2 * radius + y, radius * 2, radius * 2, 270.0, 90.0);

	path.closeSubpath();
	return path;
}

void BoxStyle::paint(QPainter* painter, int xOffset, int yOffset, int contentBoxWidth, int contentBoxHeight) const
{
//	if ( (background_.style() == Qt::NoBrush || background_.style() == Qt::SolidPattern)
//			&& (outline_.style() == Qt::NoPen || outline_.style() == Qt::SolidLine))
//		optimizedPaint(painter, xOffset, yOffset, contentBoxWidth, contentBoxHeight);
//	else
		unoptimizedPaint(painter, xOffset, yOffset, contentBoxWidth, contentBoxHeight);
}

void BoxStyle::unoptimizedPaint(QPainter* painter, int xOffset, int yOffset, int contentBoxWidth,
				int contentBoxHeight) const
{
	qreal x = xOffset;
	qreal y = yOffset;

	int outlineWidth = outline_.style()!=Qt::NoPen ? outline_.width() : 0;
	// Move the figure when using antialiasing. The outline will start at a pixel boundary. This makes it sharper.
	if ( painter->testRenderHint(QPainter::Antialiasing) || painter->testRenderHint(QPainter::HighQualityAntialiasing) )
		if ( outline().style() != Qt::NoPen)
		{
			x = xOffset + outlineWidth/2.0;
			y = yOffset + outlineWidth/2.0;
		}

	// Draw box.
	painter->setPen(outline());

	// Set the brush and fix the gradient if needed.
	if ( background().style() == Qt::LinearGradientPattern
			&& background().gradient()->coordinateMode() == QGradient::LogicalMode )
	{
		QLinearGradient g = *(static_cast<const QLinearGradient*> (background().gradient()));
		g.setStart(x + g.start().x(), y + g.start().y());
		g.setFinalStop(x + g.finalStop().x(), y + g.finalStop().y());
		painter->setBrush(g);

	}
	else if ( background().style()  == Qt::RadialGradientPattern
			&& background().gradient()->coordinateMode() == QGradient::LogicalMode )
	{
		QRadialGradient g = *(static_cast<const QRadialGradient*> (background().gradient()));
		g.setCenter(x + g.center().x(), y + g.center().y());
		g.setFocalPoint(x + g.focalPoint().x(), y + g.focalPoint().y());
		painter->setBrush(g);
	}
	else
	{
		painter->setBrush(background());
	}

	painter->drawPath(getRectanglePath(x, y, contentBoxWidth - outlineWidth, contentBoxHeight - outlineWidth));
}

void BoxStyle::optimizedPaint(QPainter* painter, int xOffset, int yOffset, int contentBoxWidth,
				int contentBoxHeight) const
{
	qreal scaleFactor = painter->worldTransform().m11();
	int outlineWidth = outline_.style()==Qt::SolidLine ? outline_.width() : 0;
	int halfOutline = std::ceil(outlineWidth/2.0);

	if (corner_ == CornerType::RightAngle || cornerRadius_*scaleFactor <= 1.0 )
	{
		int innerWidth = contentBoxWidth - 2*halfOutline;
		int innerHieght = contentBoxHeight - 2*halfOutline;

		// Note that the half of the outline overlaps the background.

		// Draw a simplified version
		if (background_.style() == Qt::SolidPattern)
			painter->fillRect(xOffset + halfOutline, yOffset + halfOutline, innerWidth, innerHieght, background_.color());

		// Paint outline
		if (outline_.style() == Qt::SolidLine)
		{
			//Top
			painter->fillRect(xOffset, yOffset, contentBoxWidth, outlineWidth, outline_.color());
			//Left
			painter->fillRect(xOffset, yOffset + outlineWidth, outlineWidth,
					contentBoxHeight-2*outlineWidth, outline_.color());
			//Bottom
			painter->fillRect(xOffset, yOffset + contentBoxHeight - outlineWidth, contentBoxWidth,
					outlineWidth, outline_.color());
			//Right
			painter->fillRect(xOffset + contentBoxWidth - outlineWidth, yOffset + outlineWidth,
					outlineWidth, contentBoxHeight-2*outlineWidth, outline_.color());
		}

		return;
	}

	int subImageSize = cornerRadius_ + halfOutline;

	// Paint corners
	if (cornerRadius_ > 0)
	{
		if (scaleFactor*cornerRadius_ >= 1)
		{
			// Draw the corners in case there are at least one pixel
			bool hasScale = topLeftCorner_.paint(painter, xOffset, yOffset);
			if (!hasScale)
			{
				generatePixmaps(scaleFactor, painter);
				topLeftCorner_.paint(painter, xOffset, yOffset);
			}

			int rightCornerStart = contentBoxWidth - subImageSize;
			int bottomCornerStart = contentBoxHeight - subImageSize;

			topRightCorner_.paint(painter, xOffset + rightCornerStart, yOffset);
			bottomLeftCorner_.paint(painter, xOffset, yOffset + bottomCornerStart);
			bottomRightCorner_.paint(painter, xOffset + rightCornerStart, yOffset + bottomCornerStart);
		}
	}

	int innerWidth = contentBoxWidth - 2*subImageSize;
	int innerHeight = contentBoxHeight - 2*subImageSize;

	// Paint background
	if (background_.style() == Qt::SolidPattern)
	{
		// vertical middle
		painter->fillRect(xOffset + subImageSize, yOffset + outlineWidth,
				innerWidth, contentBoxHeight - 2*outlineWidth, background_.color());
		// left
		painter->fillRect(xOffset + outlineWidth, yOffset + subImageSize,
				subImageSize - outlineWidth, innerHeight, background_.color());
		// right
		painter->fillRect(xOffset + contentBoxWidth - subImageSize, yOffset + subImageSize,
				subImageSize - outlineWidth, innerHeight, background_.color());
	}

	// Paint outline
	if (outline_.style() == Qt::SolidLine)
	{
		// top
		painter->fillRect(xOffset + subImageSize, yOffset,
				innerWidth, outlineWidth, outline_.color());
		// bottom
		painter->fillRect(xOffset + subImageSize, yOffset + contentBoxHeight - outlineWidth,
				innerWidth, outlineWidth, outline_.color());
		// left
		painter->fillRect(xOffset , yOffset + subImageSize,
				outlineWidth, innerHeight, outline_.color());
		// right
		painter->fillRect(xOffset + contentBoxWidth - outlineWidth, yOffset + subImageSize,
				outlineWidth, innerHeight, outline_.color());
	}
}

void BoxStyle::generatePixmaps(qreal scaleFactor, const QPainter* painterSpecifyingRenderHints) const
{
	qreal x = 0;
	qreal y = 0;

	int outlineWidth = 0;
	if ( outline_.style() != Qt::NoPen)
	{
		outlineWidth = outline_.width();
		x = outlineWidth / 2.0;
		y = outlineWidth / 2.0;
	}

	// Draw a square.
	int extra = outlineWidth; // The is just to give us some buffer in between the corners.
	int squareSize = extra + 2*cornerRadius_ + outlineWidth;
	int scaledSquare = std::ceil(squareSize * scaleFactor);
	QImage img = QImage(QSize(scaledSquare,scaledSquare), QImage::Format_ARGB32);
	img.fill(0);

	QPainter painter(&img);
	painter.scale(scaleFactor,scaleFactor);
	painter.setRenderHints(painterSpecifyingRenderHints->renderHints());
	painter.setPen(outline_);
	painter.setBrush(background_);
	painter.drawPath(getRectanglePath(x, y, squareSize - outlineWidth, squareSize - outlineWidth));

	int subImageSize = scaleFactor*(cornerRadius_ + std::ceil(outlineWidth/2.0));
	int endStarts = scaleFactor*squareSize - subImageSize;
	topLeftCorner_.setImage(img.copy(0,0,subImageSize,subImageSize), scaleFactor);
	topRightCorner_.setImage(img.copy(endStarts,0,subImageSize,subImageSize), scaleFactor);
	bottomLeftCorner_.setImage(img.copy(0,endStarts,subImageSize,subImageSize), scaleFactor);
	bottomRightCorner_.setImage(img.copy(endStarts,endStarts,subImageSize,subImageSize), scaleFactor);
}

}
