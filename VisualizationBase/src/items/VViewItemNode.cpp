/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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
#include "VViewItemNode.h"

#include "../declarative/DeclarativeItemDef.h"
#include "../nodes/ViewItemNode.h"
#include "../declarative/GridLayoutFormElement.h"
#include "../items/ViewItem.h"

namespace Visualization {

DEFINE_ITEM_COMMON(VViewItemNode, "item")

VViewItemNode::VViewItemNode(Item* parent, NodeType* node, const StyleType* style) :
		Super{parent, node, style}
{
	setPurpose(node->purpose());
}

void VViewItemNode::initializeForms()
{
	//We either visualize the target form, or have only spacing
	addForm(item(&I::reference_, [](I* v) { return v->node()->reference(); }));
	addForm(item(&I::spacing_, [](I* v) { return v->style(); }));
}

int VViewItemNode::determineForm()
{
	if (node()->reference()) return 0;
	else return 1;
}

bool VViewItemNode::determineSpacing()
{
	Q_ASSERT(node()->reference() == nullptr);
	if (auto target = node()->spacingTarget())
	{
		//The spacing could be in another view item (if we add an item to a not open view item)
		//Then the spacing will be updated when the view item is opened
		auto parentItem = node()->spacingParent()
				  ? scene()->currentViewItem()->findVisualizationOf(node()->spacingParent())
				  : scene()->currentViewItem();
		if (parentItem)
			if (auto targetItem = parentItem->findVisualizationOf(target))
			{
				auto curYPos = scenePos().y();
				auto curTargetYPos = targetItem->scenePos().y();
				auto height = curTargetYPos - curYPos - 10;
				height = height > 0 ? height : 0;
				if (height != spacing_->heightInParent())
				{
					spacing_->setCustomSize(50, height);
					setUpdateNeeded(RepeatUpdate);
					return true;
				}
			}
	}
	else if (DEFAULT_SPACING_HEIGHT != spacing_->heightInParent())
	{
		spacing_->setCustomSize(50, DEFAULT_SPACING_HEIGHT);
		setUpdateNeeded(RepeatUpdate);
		return true;
	}
	return false;
}

}
