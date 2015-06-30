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
#include "VViewSwitcherNode.h"

#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "vis/TextAndDescription.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/ViewItem.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(VViewSwitcherNode, "item")

VViewSwitcherNode::VViewSwitcherNode(Visualization::Item* parent, NodeType* node, const StyleType* style) :
		Super(parent, node, style)
{
	initNonEditable();
}

void VViewSwitcherNode::initializeForms()
{
	addForm(item(&I::nonEditable_));
	addForm(item(&I::editable_));
}

void VViewSwitcherNode::setNameEditable(bool editable)
{
	if (!editable)
	{
		scene()->viewItem(node()->viewName())->setName(editable_->text());
		node()->setViewName(editable_->text());
	}
	//TODO@cyril Is there a way to make this initialization automatic? Or not always destroy the items?
	if (editable && editable != nameEditable_)
		initEditable();
	else if (editable != nameEditable_)
		initNonEditable();
	nameEditable_ = editable;
}

int VViewSwitcherNode::determineForm()
{
	return nameEditable_ ? 1 : 0;
}

void VViewSwitcherNode::initNonEditable()
{
	nonEditable_ = new TextAndDescription(this);
	nonEditable_->setContents(node()->viewName(), QString());
}

void VViewSwitcherNode::initEditable()
{
	editable_ = new Visualization::Text(this, node()->viewName());
	editable_->setEditable(true);
}

}
