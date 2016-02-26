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

#include "CursorShapeItem.h"
#include "Cursor.h"
#include "../shapes/Shape.h"

namespace Visualization {

DEFINE_ITEM_COMMON(CursorShapeItem, "item")

CursorShapeItem::CursorShapeItem(Cursor* cursor, const StyleType* style) :
Super{nullptr, style}, cursor_{cursor}, useCenter_{false}
{
	setFlags(0);
	setAcceptedMouseButtons(0);
	setZValue(LAYER_CURSOR_Z);
	setItemCategory(Scene::CursorItemCategory);
}

CursorShapeItem::~CursorShapeItem()
{
}

Item::UpdateType CursorShapeItem::needsUpdate()
{
	return StandardUpdate;
}

void CursorShapeItem::determineChildren()
{

}

void CursorShapeItem::updateGeometry(int, int)
{
	if (hasShape())
	{
		getShape()->setInnerSize(size_.width(), size_.height());
		QPointF ref = useCenter_ ? center_ - QPoint{ getShape()->contentLeft(), getShape()->contentTop() } : topLeft_;
		auto scalingFactor = mainViewScalingFactor();

		// Adjust for the scaling if this cursor should ignore transformations
		if ((flags() & ItemIgnoresTransformations) && scalingFactor != 1.0)
		{
			// TODO: While this does work, one might wonder if it is the best idea. Things get a bit too coupled.

			// The top-left corner of the topmost parent of owner() is relative to where we should draw the cursor
			// as this location is scaled normally regardless of ItemIgnoresTransformations
			auto topmost = cursor_->owner();
			while (topmost->parent()) topmost = topmost->parent();

			// The difference to the top-left corner must be scaled with the reverse factor
			ref += cursor_->owner()->scenePos() - topmost->scenePos();
			ref /= scalingFactor;
			ref += topmost->scenePos();

			setPos(ref);
		}
		else setPos(cursor_->owner()->scenePos() + ref);
	}
}

void CursorShapeItem::setCursorCenter(const QPoint& center)
{
	center_ = center;
	useCenter_=true;
}

void CursorShapeItem::setCursorTopLeft(const QPoint& topLeft)
{
	topLeft_ = topLeft;
	useCenter_=false;
}

}
