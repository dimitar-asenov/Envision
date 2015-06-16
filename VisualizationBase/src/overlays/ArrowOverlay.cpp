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

namespace Visualization {

ITEM_COMMON_DEFINITIONS(ArrowOverlay, "item")

ArrowOverlay::ArrowOverlay(Item* arrowFrom, Item* arrowTo, const StyleType* style)
	: Super{{arrowFrom, arrowTo}, style}
{
}

void ArrowOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->drawLine(lineFrom_, lineTo_);
}

void ArrowOverlay::determineChildren(){}

void ArrowOverlay::updateGeometry(int, int)
{
	Item* left{};
	Item* right{};
	if (firstAssociatedItem()->scenePos().x() < secondAssociatedItem()->scenePos().x())
	{
		left = firstAssociatedItem();
		right = secondAssociatedItem();
	}
	else
	{
		left = secondAssociatedItem();
		right = firstAssociatedItem();
	}
	lineFrom_ = QPoint(left->scenePos().x() + left->widthInLocal(),
					   left->scenePos().y() + left->heightInLocal() / 2);
	lineTo_ = QPoint(right->scenePos().x(),
					 right->scenePos().y() + right->heightInLocal() / 2);

	setPos(0, 0);
	setSize(std::max(lineFrom_.x(), lineTo_.x()),
			 std::max(lineFrom_.y(), lineTo_.y()));
}

}
