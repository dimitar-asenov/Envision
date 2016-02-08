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

#include "FrameStyle.h"
#include "BoxStyle.h"

namespace Visualization {

FrameStyle::~FrameStyle(){}

void FrameStyle::load(StyleLoader& sl)
{
	Super::load(sl);
	// Create the gradient of the shadow
	int maxShadowSize = std::max(topLeftShadowWidth(), bottomRightShadowWidth());
	if (maxShadowSize > 0)
	{
		std::vector<QColor> linearShadowGradient_;
		for (int i = maxShadowSize; i >= 0 ; --i)
			linearShadowGradient_.push_back(shadowColorAt(maxShadowSize, i));

		shadowGradient_.push_back(linearShadowGradient_.front());
		for (int i = 1; i <= maxShadowSize; ++i)
		{
			shadowGradient_.push_back(
						colorNeededForBlending(linearShadowGradient_[i-1], linearShadowGradient_[i]));
		}
	}
}

QColor FrameStyle::shadowColorAt(qreal shadowWidth, qreal pixelswayFromFrame) const
{
	if (pixelswayFromFrame <= 0)
		return shadowStartColor();

	if (pixelswayFromFrame >= shadowWidth)
		return shadowStopColor();

	qreal ratio = pixelswayFromFrame / shadowWidth;
	int red = ratio*shadowStopColor().red() + (1-ratio)*shadowStartColor().red();
	int green = ratio*shadowStopColor().green() + (1-ratio)*shadowStartColor().green();
	int blue = ratio*shadowStopColor().blue() + (1-ratio)*shadowStartColor().blue();
	int alpha  = ratio*shadowStopColor().alpha() + (1-ratio)*shadowStartColor().alpha();

	return {red, green, blue, alpha};
}

QColor FrameStyle::colorNeededForBlending(QColor destinationColor, QColor desiredColor)
{
	qreal alpha = (desiredColor.alphaF() - destinationColor.alphaF() ) / (1 - destinationColor.alphaF());
	qreal red = (desiredColor.redF()*(alpha*desiredColor.alphaF()*(1-alpha))
			- destinationColor.redF()*destinationColor.alphaF()*(1-alpha) )
			/ alpha;
	qreal green = (desiredColor.greenF()*(alpha*desiredColor.alphaF()*(1-alpha))
			- destinationColor.greenF()*destinationColor.alphaF()*(1-alpha) )
			/ alpha;
	qreal blue = (desiredColor.blueF()*(alpha*desiredColor.alphaF()*(1-alpha))
			- destinationColor.blueF()*destinationColor.alphaF()*(1-alpha) )
			/ alpha;

	return QColor::fromRgbF(red, green, blue, alpha);
}

void FrameStyle::paint(QPainter* painter, int xOffset, int yOffset, int outerWidth, int outerHeight,
							QColor customColor) const
{
	auto topWidth = this->topWidth();
	auto bottomWidth = this->bottomWidth();
	auto leftWidth = this->leftWidth();
	auto rightWidth = this->rightWidth();

	// Adjust the widths to expand them at small scales
	// This makes frames look more discernable
	qreal scaleFactor = painter->worldTransform().m11();
	if (scaleFactor < 0.2)
	{
		if (topWidth > 0 && topWidth*scaleFactor < 1) topWidth = 1 / scaleFactor;
		if (bottomWidth > 0 && bottomWidth*scaleFactor < 1) bottomWidth = 1 / scaleFactor;
		if (leftWidth > 0 && leftWidth*scaleFactor < 1) leftWidth = 1 / scaleFactor;
		if (rightWidth > 0 && rightWidth*scaleFactor < 1) rightWidth = 1 / scaleFactor;
	}

	// Compute size excluding shadow
	int widthWithoutShadow = outerWidth - topLeftShadowWidth() - bottomRightShadowWidth();
	int heightWithoutShadow = outerHeight - topLeftShadowWidth() - bottomRightShadowWidth();

	// Compute offsets excluding shadow
	int xAfterShadow = xOffset + topLeftShadowWidth();
	int yAfterShadow = yOffset + topLeftShadowWidth();

	// Draw shadows
	int maxShadowSize = std::max(topLeftShadowWidth(), bottomRightShadowWidth());
	if (maxShadowSize > 0 && maxShadowSize * scaleFactor >= 0.5)
	{
		// The shadow has to be drawn in this way and not using QPainter::fillRect, since fillRect
		// causes weird artifacts (darker pixels) at the bounderies of the filled rectangles

		// Clip the middle
		QPainterPath clipPath;
		clipPath.addRect(QRect{xOffset, yOffset, outerWidth, outerHeight});
		QPainterPath innerBox;
		innerBox.addRect(QRect{xAfterShadow, yAfterShadow, widthWithoutShadow, heightWithoutShadow});
		clipPath -= innerBox;
		painter->setClipPath(clipPath);

		painter->setPen(Qt::NoPen);
		painter->setBrush(QColor{0, 0, 0, 5});

		for (int i = 0; i < maxShadowSize; ++i)
		{
			painter->setBrush(shadowGradient_[i]);
			painter->drawRoundedRect(xOffset + i, yOffset + i, outerWidth - 2*i, outerHeight - 2*i,
											 maxShadowSize-i, maxShadowSize-i);
		}

		painter->setClipping(false);
	}

	// Draw the frame borders
	int adjustedTopWidth = std::min(topWidth, heightWithoutShadow);

	if (adjustedTopWidth > 0 && heightWithoutShadow > 0)
		painter->fillRect(xAfterShadow, yAfterShadow, widthWithoutShadow, adjustedTopWidth,
								BoxStyle::fixGradient(topBrush(), xAfterShadow, yAfterShadow));

	int adjustedBottomWidth = std::min(bottomWidth, heightWithoutShadow - adjustedTopWidth);
	int contentHeight = heightWithoutShadow - adjustedTopWidth - adjustedBottomWidth;
	if (adjustedBottomWidth > 0 && heightWithoutShadow > 0)
		painter->fillRect(xAfterShadow, yAfterShadow + adjustedTopWidth + contentHeight,
								widthWithoutShadow, adjustedBottomWidth,
								BoxStyle::fixGradient(bottomBrush(), xAfterShadow,
															 yAfterShadow + adjustedTopWidth + contentHeight));

	int adjustedLeftWidth = std::min(leftWidth, widthWithoutShadow);
	if (adjustedLeftWidth > 0 && contentHeight > 0)
		painter->fillRect(xAfterShadow, yAfterShadow + adjustedTopWidth, adjustedLeftWidth, contentHeight,
								BoxStyle::fixGradient(leftBrush(), xAfterShadow, yAfterShadow + adjustedTopWidth));

	int adjustedRightWitdh = std::min(rightWidth, widthWithoutShadow - adjustedLeftWidth);
	int contentWidth = widthWithoutShadow - adjustedLeftWidth - adjustedRightWitdh;
	if (adjustedRightWitdh > 0 && contentHeight > 0)
		painter->fillRect(xAfterShadow + adjustedLeftWidth + contentWidth, yAfterShadow + adjustedTopWidth,
								adjustedRightWitdh, contentHeight,
								BoxStyle::fixGradient(rightBrush(), xAfterShadow + adjustedLeftWidth + contentWidth,
															 yAfterShadow + adjustedTopWidth));

	if (contentWidth > 0 && contentHeight > 0)
	{
		if (customColor.isValid())
			painter->fillRect(xAfterShadow + adjustedLeftWidth, yAfterShadow + adjustedTopWidth,
									contentWidth, contentHeight, customColor);
		else
			painter->fillRect(xAfterShadow + adjustedLeftWidth, yAfterShadow + adjustedTopWidth,
									contentWidth, contentHeight,
									BoxStyle::fixGradient(contentBrush(), xAfterShadow + adjustedLeftWidth,
																 yAfterShadow + adjustedTopWidth));
	}
}

}
