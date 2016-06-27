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

#include "NodeWrapper.h"
#include "../shapes/Shape.h"

namespace Visualization {

DEFINE_ITEM_COMMON(NodeWrapper, "item")

NodeWrapper::NodeWrapper(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

NodeWrapper::~NodeWrapper()
{
	SAFE_DELETE(wrappedItem_);
}

void NodeWrapper::determineChildren()
{
	synchronizeItem(wrappedItem_, node());
}

bool NodeWrapper::isEmpty() const
{
	if (stretchable_ && !node()) return false;
	else if (wrappedItem_) return wrappedItem_->isEmpty();
	else return true;
}

bool NodeWrapper::hasNode() const
{
	// TODO: should not need to redefine this
	return node() != nullptr;
}

bool NodeWrapper::sizeDependsOnParent() const
{
	if (stretchable_ && !node()) return true;
	else if (wrappedItem_) return wrappedItem_->sizeDependsOnParent();
	else return false;
}

void NodeWrapper::updateGeometry(int availableWidth, int availableHeight)
{
	if (stretchable_ && !node())
	{
		getShape()->setOffset(0, 0);
		getShape()->setOutterSize(availableWidth, availableHeight);
		setSize(availableWidth, availableHeight);
		if (zValue() != -1 ) setZValue(-1);
	}
	else
	{
		Item::updateGeometry(wrappedItem_, availableWidth, availableHeight);
		if (zValue() != 0) setZValue(0);
	}
}

void NodeWrapper::refreshWrappedItem()
{
	SAFE_DELETE(wrappedItem_);
	setUpdateNeeded(StandardUpdate);
}

}
