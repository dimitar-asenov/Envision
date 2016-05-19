/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "VCommentedNode.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/model/AllTreeManagers.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/items/Item.h"

namespace CodeReview
{

DEFINE_ITEM_COMMON(VCommentedNode, "item")

VCommentedNode::VCommentedNode(Visualization::Item* parent, NodeType* node, const StyleType* style)
	: Super{parent, node, style}
{
}

void VCommentedNode::initializeForms()
{
	auto comments = item(&I::comments_, [](I* v) {
			return v->node()->commentWithDateNodes();});
	auto grid = (new Visualization::GridLayoutFormElement{})
			->setHorizontalSpacing(50)
			->setLeftMargin(10)
			->setRightMargin(10)
			->setHorizontalAlignment(Visualization::LayoutStyle::Alignment::Center)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
			->setColumnStretchFactor(0, 1)
			->put(0, 0, comments);

	addForm(grid);
}
}
