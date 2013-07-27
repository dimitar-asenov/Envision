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

#include "MarkdownLineItem.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(MarkdownLineItem, "item")

MarkdownLineItem::MarkdownLineItem(Item* parent, const StyleType* style, int strength) : Super(parent, style)
{
	setFlag(QGraphicsItem::ItemHasNoContents, false);
	strength_ = std::min(1, std::max(3, strength));
}

void MarkdownLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if (width() > height())
		painter->drawLine(QPointF(0, height()/2.0), QPointF(width(), height()/2.0));
	else
		painter->drawLine(QPointF(width()/2.0, 0), QPointF(width()/2.0, height()));
}

void MarkdownLineItem::updateGeometry(int availableWidth, int availableHeight)
{
	int w = availableWidth > 0 ? availableWidth : 300;
	int h = std::max(availableHeight, height());
	if(h == 0) h = 10;
	setSize(w, h);
}

void MarkdownLineItem::determineChildren(){}

bool MarkdownLineItem::sizeDependsOnParent() const
{
	return true;
}

} /* namespace Comments */
