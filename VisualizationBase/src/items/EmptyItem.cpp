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
#include "EmptyItem.h"

#include "../shapes/Shape.h"

namespace Visualization {

DEFINE_ITEM_COMMON(EmptyItem, "item")

EmptyItem::EmptyItem(Item* parent, const StyleType* style) : Super{parent, style}
{
	setZValue(-1);
}

void EmptyItem::setCustomSize(int width, int height)
{
	if (hasCustomSize_ && width == widthInLocal() && height == heightInLocal()) return;

	auto shape = getShape();

	if (shape)
		shape->setOuterSize(width, height);
	else
		setSize(width, height);

	hasCustomSize_ = true;
}

void EmptyItem::determineChildren(){}

bool EmptyItem::sizeDependsOnParent() const
{
	return !hasCustomSize_;
}

void EmptyItem::updateGeometry(int availableWidth, int availableHeight)
{
	if (hasCustomSize_) return;

	auto shape = getShape();

	if (shape)
	{
		if (availableWidth == 0 && availableHeight == 0)
			shape->setInnerSize(2, 2);
		else
		{
			shape->setOuterSize(availableWidth, availableHeight);
			setSize(availableWidth, availableHeight);
		}
	}
	else
		setSize(1, 1);
}

}
