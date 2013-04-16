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

#include "RootItem.h"
#include "ItemStyle.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(RootItem, "item")

RootItem::RootItem(Item* parent, NodeType* node, int purpose)
	: BaseItemType(nullptr, node, itemStyles().get()), item_(nullptr)
{
	Q_ASSERT(parent == nullptr);
	setFlag(QGraphicsItem::ItemHasNoContents);
	setItemCategory(Scene::CodeItemCategory);
	setPurpose(purpose);
}

RootItem::RootItem(NodeType *node, int purpose) : BaseItemType(nullptr, node, itemStyles().get()), item_(nullptr)
{
	setFlag(QGraphicsItem::ItemHasNoContents);
	setItemCategory(Scene::CodeItemCategory);
	setPurpose(purpose);
}

RootItem::RootItem(Item* item, int purpose) : BaseItemType(nullptr, nullptr, itemStyles().get()), item_(item)
{
	setFlag(QGraphicsItem::ItemHasNoContents);
	setPurpose(purpose);
	if (item_) item_->setParentItem(this);
}

RootItem::~RootItem()
{
	SAFE_DELETE_ITEM(item_);
}

void RootItem::setItem(Item* item)
{
	item_ = item;
	if (item_) item_->setParentItem(this);
	setUpdateNeeded(StandardUpdate);
}

void RootItem::updateGeometry(int availableWidth, int availableHeight)
{
	if (item_) Item::updateGeometry(item_, availableWidth, availableHeight);
	else setSize(QSize(0,0));
}

void RootItem::determineChildren()
{
	if (node() && needsUpdate() == FullUpdate) SAFE_DELETE_ITEM(item_);

	if (item_ && node() && item_->node() != node() ) SAFE_DELETE_ITEM(item_);

	if (!item_ && node()) item_ = renderer()->render(this, node());
}

} /* namespace Visualization */
