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

	int adjustedTopWidth = std::min(topWidth, outerHeight);

	if (adjustedTopWidth > 0 && outerHeight > 0)
		painter->fillRect(xOffset, yOffset, outerWidth, adjustedTopWidth,
							BoxStyle::fixGradient(topBrush(), xOffset, yOffset));

	int adjustedBottomWidth = std::min(bottomWidth, outerHeight - adjustedTopWidth);
	int contentHeight = outerHeight - adjustedTopWidth - adjustedBottomWidth;
	if (adjustedBottomWidth > 0 && outerHeight > 0)
		painter->fillRect(xOffset, yOffset + adjustedTopWidth + contentHeight, outerWidth, adjustedBottomWidth,
							BoxStyle::fixGradient(bottomBrush(), xOffset, yOffset + adjustedTopWidth + contentHeight));

	int adjustedLeftWidth = std::min(leftWidth, outerWidth);
	if (adjustedLeftWidth > 0 && contentHeight > 0)
		painter->fillRect(xOffset, yOffset + adjustedTopWidth, adjustedLeftWidth, contentHeight,
							BoxStyle::fixGradient(leftBrush(), xOffset, yOffset + adjustedTopWidth));

	int adjustedRightWitdh = std::min(rightWidth, outerWidth - adjustedLeftWidth);
	int contentWidth = outerWidth - adjustedLeftWidth - adjustedRightWitdh;
	if (adjustedRightWitdh > 0 && contentHeight > 0)
		painter->fillRect(xOffset + adjustedLeftWidth + contentWidth, yOffset + adjustedTopWidth,
							adjustedRightWitdh, contentHeight,
							BoxStyle::fixGradient(rightBrush(), xOffset + adjustedLeftWidth + contentWidth,
														 yOffset + adjustedTopWidth));

	if (contentWidth > 0 && contentHeight > 0)
	{
		if (customColor.isValid())
			painter->fillRect(xOffset + adjustedLeftWidth, yOffset + adjustedTopWidth, contentWidth, contentHeight,
									customColor);
		else
			painter->fillRect(xOffset + adjustedLeftWidth, yOffset + adjustedTopWidth, contentWidth, contentHeight,
							BoxStyle::fixGradient(contentBrush(), xOffset + adjustedLeftWidth, yOffset + adjustedTopWidth));
	}
}

}
