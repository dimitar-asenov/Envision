/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "VMetaCallExpression.h"
#include "VReferenceExpression.h"

#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"

#include "OOModel/src/declarations/Declaration.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VMetaCallExpression, "item")

VMetaCallExpression::VMetaCallExpression(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style}
{}

VMetaCallExpression::~VMetaCallExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	prefix_ = nullptr;
	callee_ = nullptr;
	arguments_ = nullptr;
	generated_ = nullptr;
}

void VMetaCallExpression::determineChildren()
{
	layout()->synchronizeFirst(prefix_, true, &style()->prefix());

	if (auto ref = DCast<ReferenceExpression>(node()->callee()))
	{
		// TODO: Find a way around that ugly hack. It might eve
		layout()->synchronizeMid<Item, VReferenceExpression>(callee_, ref, &style()->name(), 1);

		if (callee_) static_cast<VReferenceExpression*>(callee_)->setStyle( &style()->name());
	}
	else
		layout()->synchronizeMid(callee_, node()->callee(), 1);

	layout()->synchronizeMid(arguments_, node()->arguments(), &style()->arguments(), 2);

	// At the moment the line below is broken, since generation fails.
	//layout()->synchronizeLast(generated_, node()->generatedTree());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	prefix_->setStyle( &style()->prefix());
	arguments_->setStyle( &style()->arguments() );
	arguments_->setSuppressDefaultRemovalHandler(true);
}

}
