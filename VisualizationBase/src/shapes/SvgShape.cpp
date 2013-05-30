/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "SvgShape.h"
#include "VisualizationException.h"

namespace Visualization {

SHAPE_COMMON_DEFINITIONS(SvgShape, "shape")

SvgShape::SvgShape(Item *parent, StyleType *style) :
	Super(parent, style)
{
}

void SvgShape::update()
{
	if ( sizeSpecified() == InnerSize )
	{
		svgWidth = width() /
				(1.0 - style()->leftContentMarginFraction() - style()->rightContentMarginFraction());
		svgHeight = height() /
				(1.0 - style()->topContentMarginFraction() - style()->bottomContentMarginFraction());
	}
	else
	{
		svgWidth = width();
		svgHeight = height();
	}

	setItemSize(xOffset() + std::ceil(svgWidth), yOffset() + std::ceil(svgHeight));
}

int SvgShape::contentLeft()
{
	int outterWidth = sizeSpecified() == OutterSize ? width() : Super::outterSize(width(), height()).width();
	return xOffset() + std::ceil(outterWidth*style()->leftContentMarginFraction());
}

int SvgShape::contentTop()
{
	int outterHeight = sizeSpecified() == OutterSize ? height() : Super::outterSize(width(), height()).height();
	return yOffset() + std::ceil(outterHeight*style()->topContentMarginFraction());
}

QSize SvgShape::innerSize(QSize outterSize) const
{
	int innerWidth = (1.0 - style()->leftContentMarginFraction() - style()->rightContentMarginFraction())
			*outterSize.width();
	int innerHeight = (1.0 - style()->topContentMarginFraction() - style()->bottomContentMarginFraction())
			*outterSize.height();

	return QSize(innerWidth, innerHeight);
}

QSize SvgShape::outterSize(QSize innerSize) const
{
	int outterWidth = innerSize.width() /
			(1.0 - style()->leftContentMarginFraction() - style()->rightContentMarginFraction());
	int outterHeight = innerSize.height() /
			(1.0 - style()->topContentMarginFraction() - style()->bottomContentMarginFraction());

	return QSize(outterWidth, outterHeight);
}

void SvgShape::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	qreal x = xOffset();
	qreal y = yOffset();

	style()->paint(painter, x, y, svgWidth, svgHeight);
}

} /* namespace Visualization */
