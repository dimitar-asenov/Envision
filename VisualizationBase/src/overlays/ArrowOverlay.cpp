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

#include "ArrowOverlay.h"
#include "../utils/Drawing.h"
#include "../VisualizationManager.h"

namespace Visualization {

DEFINE_ITEM_COMMON(ArrowOverlay, "item")

ArrowOverlay::ArrowOverlay(Item* arrowFrom, Item* arrowTo, const StyleType* style)
	: Super{{arrowFrom, arrowTo}, style}
{
	setAcceptHoverEvents(true);
	selected_ = false;
}

void ArrowOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if (selected_)
		currentArrowOutline_ = Drawing::drawArrow(painter, lineFrom_, lineTo_, style()->selectedArrowBrush(),
																style()->selectedLinePen(), invertArrow_, !invertArrow_,
																style()->selectedWidth());
	else
		currentArrowOutline_ = Drawing::drawArrow(painter, lineFrom_, lineTo_, style()->arrowBrush(),
																style()->linePen(), invertArrow_, !invertArrow_,
																style()->width());
}

void ArrowOverlay::determineChildren(){}

void ArrowOverlay::updateGeometry(int, int)
{
	invertArrow_ = false;
	//Find the space the line will occupy in the scene
	auto first = firstAssociatedItem();
	auto second = secondAssociatedItem();

	auto leftTopCorner = QPointF{
				std::min(first->scenePos().x() + first->widthInScene(),
							second->scenePos().x() + second->widthInScene()),
				std::min(first->scenePos().y() + first->heightInScene() / 2,
							second->scenePos().y() + second->heightInScene() / 2)}.toPoint();
	auto rightBottomCorner = QPointF{
				std::max(first->scenePos().x(), second->scenePos().x()),
				std::max(first->scenePos().y() + first->heightInScene() / 2,
							second->scenePos().y() + second->heightInScene() / 2)}.toPoint();

	setPos(leftTopCorner.x(), leftTopCorner.y());
	setSize(rightBottomCorner.x() - leftTopCorner.x(),
			rightBottomCorner.y() - leftTopCorner.y());

	//Find the actual starting and end points of the line in local coordinates
	if (first->scenePos().x() > second->scenePos().x())
	{
		auto temp = second;
		second = first;
		first = temp;
		invertArrow_ = true;
	}
	lineFrom_ = QPointF{first->scenePos().x() + first->widthInScene() - leftTopCorner.x(),
						first->scenePos().y() + first->heightInScene() / 2 - leftTopCorner.y()}.toPoint();
	lineTo_ = QPointF{second->scenePos().x() - leftTopCorner.x(),
					 second->scenePos().y() + second->heightInScene() / 2 - leftTopCorner.y()}.toPoint();
}

QPainterPath ArrowOverlay::shape() const
{
	QPainterPath painterPath;
	painterPath.addPolygon(currentArrowOutline_);
	return painterPath;
}

}
