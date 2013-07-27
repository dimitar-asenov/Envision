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

#include "MarkdownTextItem.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(MarkdownTextItem, "item")

MarkdownTextItem::MarkdownTextItem(Item* parent, const StyleType* style, QString str)
	: Super(parent, style)
{
	setFlag(QGraphicsItem::ItemHasNoContents, false);
	text_ = QStaticText(str);
}

void MarkdownTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->drawStaticText(0, 0, text_);
}

void MarkdownTextItem::updateGeometry(int availableWidth, int)
{
	int w = availableWidth == 0 ? 300 : availableWidth;
	text_.setTextWidth(w);
	setSize(text_.size());
}

void MarkdownTextItem::determineChildren(){}

bool MarkdownTextItem::sizeDependsOnParent() const
{
	return true;
}


} /* namespace Comments */
