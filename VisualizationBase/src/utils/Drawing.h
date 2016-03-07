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

#pragma once

#include "../visualizationbase_api.h"

class VISUALIZATIONBASE_API Drawing
{
	public:
		/**
		 * @brief Draws an arrow on screen.
		 * @param painter The QPainter to use for the drawing
		 * @param begin The start point of the arrow
		 * @param end The end point of the arrow
		 * @param arrowPen The pen to use for drawing the arrow head
		 * @param linePen The pen to use for drawing the line
		 * @param startArrow Is there an arrow to the start point?
		 * @param endArrow Is there an arrow to the end point?
		 * @param outlineSize The size of the arrow head
		 */
		static void drawArrow(QPainter* painter, QPointF begin, QPointF end, const QPen& arrowPen, const QPen& linePen,
							  bool startArrow, bool endArrow, int outlineSize);

	private:
		static QPointF drawHead(QPainter* painter, QMatrix matrix, QPolygonF arrowHead, QPointF beginOrEnd, double angle);
};
