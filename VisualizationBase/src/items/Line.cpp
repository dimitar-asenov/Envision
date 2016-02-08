/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "Line.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(Line, "item")

Line::Line(Item* parent, const LineStyle* style)
	: Super{parent, style}
{}

void Line::determineChildren(){}

void Line::updateGeometry(int availableWidth, int availableHeight)
{
	// If both height and width are specified, we'll get a fixed sized line along the longer of the two.
	// If only one is specified, this will be the fixed side of the line and the other side will stretch to fill all
	// available space.

	int w = 1;
	if (style()->width() ) w = style()->width();
	else if (availableWidth > 0) w = availableWidth;

	int h = 1;
	if (style()->height() ) h = style()->height();
	else if (availableHeight > 0) h = availableHeight;

	setSize(w, h);
}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(style()->pen());
	// Note that we use the bounding box here an not the style on purpose. See updateGeometry().
	if (widthInLocal() > heightInLocal())
		painter->drawLine(QPointF{0, heightInLocal()/2.0}, QPointF{(qreal) widthInLocal(), heightInLocal()/2.0});
	else painter->drawLine(QPointF{widthInLocal()/2.0, 0}, QPointF{widthInLocal()/2.0, (qreal) heightInLocal()});
}

bool Line::sizeDependsOnParent() const
{
	return style()->width() == 0 || style()->height() == 0;
}

}
