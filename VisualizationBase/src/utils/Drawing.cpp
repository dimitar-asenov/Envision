/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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
#include "Drawing.h"

QPolygonF Drawing::arrowSelectionPolygon(double angle, QPointF begin, QPointF end)
{

	const int selectionWidth = 20;
	QPolygonF arrowSeletionOutline;
	qreal radAngle = angle* M_PI / 180;
	qreal xDistance = selectionWidth * sin(radAngle);
	qreal yDistance = selectionWidth * cos(radAngle);
	QPointF posOffset = QPointF(xDistance, yDistance);
	QPointF negOffset = QPointF(-xDistance, -yDistance);
	arrowSeletionOutline << begin + posOffset
								<< begin + negOffset
								<< end + negOffset
								<< end + posOffset;

	return arrowSeletionOutline;
}

QPolygonF Drawing::drawArrow(QPainter* painter, QPointF begin, QPointF end, const QBrush& arrowBrush,
									  const QPen& linePen, bool startArrow, bool endArrow, int outlineSize)
{
	painter->setBrush(arrowBrush);
	painter->setPen(Qt::NoPen);

	double angle = -QLineF{begin, end}.angle();

	if (startArrow || endArrow)
	{
		QPolygonF anArrowhead;
		const int arrowTipLength = 10, arrowTipHalfWidth = 2;
		anArrowhead << QPoint{0, 0}
						<< QPoint{arrowTipLength+outlineSize, -arrowTipHalfWidth-outlineSize}
						<< QPoint{arrowTipLength+outlineSize, arrowTipHalfWidth+outlineSize};
		QMatrix matrix;
		if (startArrow)
			begin = drawHead(painter, matrix, anArrowhead, begin, angle);
		if (endArrow)
		{
			matrix.reset();
			end = drawHead(painter, matrix, anArrowhead, end, angle + 180);
		}
	}

	painter->setPen(linePen);
	painter->drawLine(begin, end);

	QPolygonF arrowSeletionOutline = arrowSelectionPolygon(-angle, end, begin);

	return arrowSeletionOutline;
}

QPointF Drawing::drawHead(QPainter *painter, QMatrix matrix, QPolygonF arrowHead, QPointF beginOrEnd, double angle)
{
	matrix.rotate(angle);
	auto arrow = matrix.map(arrowHead);
	arrow.translate(beginOrEnd);
	painter->drawPolygon(arrow);
	return QPointF{(arrow[1].x() + arrow[2].x())/2, (arrow[1].y() + arrow[2].y())/2};
}
