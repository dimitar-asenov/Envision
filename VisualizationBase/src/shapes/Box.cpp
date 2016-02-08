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

#include "Box.h"
#include "../VisualizationException.h"

namespace Visualization {

SHAPE_COMMON_DEFINITIONS(Box, "shape")

Box::Box(Item *parent, StyleType *style) : Super{parent, style}{}

int Box::contentToEdgeDistance() const
{
	int outlineWidth = style()->outline().style() != Qt::NoPen ? style()->outline().width() : 0;
	int ceilHalfRadius = (style()->cornerRadius() + 1) / 2;
	return std::max(outlineWidth, ceilHalfRadius);
}

void Box::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		auto cte = contentToEdgeDistance();
		outerWidth_ = width() + 2*cte;
		outerHeight_ = height() + 2*cte;
	}
	else
	{
		outerWidth_ = width();
		outerHeight_ = height();
	}

	setItemSize(xOffset() + outerWidth_, yOffset() + outerHeight_);
}

int Box::contentLeft()
{
	return xOffset() + contentToEdgeDistance();
}

int Box::contentTop()
{
	return yOffset() + contentToEdgeDistance();
}

QRect Box::contentRect()
{
	auto cte = contentToEdgeDistance();
	return QRect{xOffset() + cte, yOffset() + cte, outerWidth_ - 2*cte, outerHeight_ - 2*cte};
}

QSize Box::innerSize(QSize outterSize) const
{
	auto cte = contentToEdgeDistance();
	return QSize{outterSize.width() - 2*cte, outterSize.height() - 2*cte};
}

QSize Box::outterSize(QSize innerSize) const
{
	auto cte = contentToEdgeDistance();
	return QSize{innerSize.width() + 2*cte, innerSize.height() + 2*cte};
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	style()->paint(painter, xOffset(), yOffset(), outerWidth_, outerHeight_, parentItem()->customShapeColor());
}

}
