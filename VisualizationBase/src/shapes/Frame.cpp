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

#include "Frame.h"
#include "../VisualizationException.h"

namespace Visualization {

SHAPE_COMMON_DEFINITIONS(Frame, "shape")

Frame::Frame(Item *parent, StyleType *style) : Super{parent, style}{}

void Frame::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		outerWidth_ = width() + style()->leftMargin() + style()->rightMargin();
		outerHeight_ = height() + style()->topMargin() + style()->bottomMargin();
	}
	else
	{
		outerWidth_ = width();
		outerHeight_ = height();
	}

	setItemSize(xOffset() + outerWidth_, yOffset() + outerHeight_);
}

int Frame::contentLeft()
{
	return xOffset() + style()->leftMargin();
}

int Frame::contentTop()
{
	return yOffset() + style()->topMargin();
}

QRect Frame::contentRect()
{
	return {xOffset() + style()->leftMargin(), yOffset() + style()->topMargin(),
				outerWidth_ - style()->leftMargin() - style()->rightMargin(),
				outerHeight_ - style()->topMargin() - style()->bottomMargin()};
}

QSize Frame::innerSize(QSize outerSize) const
{
	return {outerSize.width() - style()->leftMargin() - style()->rightMargin(),
				outerSize.height() - style()->topMargin() - style()->bottomMargin()};
}

QSize Frame::outerSize(QSize innerSize) const
{
	return {innerSize.width() + style()->leftMargin() + style()->rightMargin(),
				innerSize.height() + style()->topMargin() + style()->bottomMargin()};
}

void Frame::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	style()->paint(painter, xOffset(), yOffset(), outerWidth_, outerHeight_, parentItem()->customShapeColor());
}

}
