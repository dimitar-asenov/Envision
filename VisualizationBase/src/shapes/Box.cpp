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

#include "shapes/Box.h"
#include "VisualizationException.h"

namespace Visualization {

SHAPE_COMMON_DEFINITIONS(Box, "shape")

Box::Box(Item *parent, StyleType *style) :
	Super(parent, style)
{
}

void Box::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		int outlineWidth = style()->outline().style() != Qt::NoPen ? style()->outline().width() : 0;
		contentBoxWidth = width() + 2 * style()->cornerRadius() + outlineWidth;
		contentBoxHeight = height() + 2 * style()->cornerRadius() + outlineWidth;
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
		setItemSize(xOffset() + contentBoxWidth, yOffset() + contentBoxHeight);
	}
	else
	{
		setItemSize(xOffset() + contentBoxWidth + style()->xShadowOffset(),
				yOffset() + contentBoxHeight + style()->yShadowOffset());
	}
}

int Box::contentLeft()
{
	int outlineWidth = style()->outline().style() != Qt::NoPen ? style()->outline().width() : 0;
	return xOffset() + style()->cornerRadius() + (outlineWidth + 1) / 2;
}

int Box::contentTop()
{
	int outlineWidth = style()->outline().style() != Qt::NoPen ? style()->outline().width() : 0;
	return yOffset() + style()->cornerRadius() + (outlineWidth + 1) / 2;
}

QSize Box::innerSize(QSize outterSize) const
{
	int outlineWidth = style()->outline().style() != Qt::NoPen ? style()->outline().width() : 0;
	int innerWidth = outterSize.width() - 2 * style()->cornerRadius() - outlineWidth;
	int innerHeight = outterSize.height() - 2 * style()->cornerRadius() - outlineWidth;

	if ( style()->shadow() != Qt::NoBrush )
	{
		innerWidth -= style()->xShadowOffset();
		innerHeight -= style()->yShadowOffset();
	}

	return QSize(innerWidth, innerHeight);
}

QSize Box::outterSize(QSize innerSize) const
{
	int outlineWidth = style()->outline().style() != Qt::NoPen ? style()->outline().width() : 0;
	int outterWidth = innerSize.width() + 2 * style()->cornerRadius() + outlineWidth;
	int outterHeight = innerSize.height() + 2 * style()->cornerRadius() + outlineWidth;

	if ( style()->shadow() != Qt::NoBrush )
	{
		outterWidth += style()->xShadowOffset();
		outterHeight += style()->yShadowOffset();
	}

	return QSize(outterWidth, outterHeight);
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	style()->paint(painter, xOffset(), yOffset(), contentBoxWidth, contentBoxHeight);
}

}
