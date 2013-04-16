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

#include "VTryCatchFinally.h"
#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VTryCatchFinally, "item")

VTryCatchFinally::VTryCatchFinally(Item* parent, NodeType* node, const StyleType* style) :
	BaseItemType(parent, node, style)
{
	layout()->setTop(true);
	tryIcon_ = new Static(layout()->top(), &style->tryIcon());
	layout()->top()->setFirst(tryIcon_);

	contents_ = new SequentialLayout(layout(), &style->contents());
	layout()->setContent(contents_);
}

VTryCatchFinally::~VTryCatchFinally()
{
	// These were automatically deleted by LayoutProvider's destructor
	tryIcon_ = nullptr;
	contents_ = nullptr;
	tryBody_ = nullptr;

	catchClauses_ = nullptr;

	finallyOutline_ = nullptr;
	finallyIcon_ = nullptr;
	finallyBody_ = nullptr;
}

void VTryCatchFinally::determineChildren()
{
	contents_->synchronizeFirst(tryBody_, node()->tryBody(), &style()->tryBody());
	contents_->synchronizeMid(catchClauses_, node()->catchClauses()->size() > 0
			?node()->catchClauses() : nullptr, &style()->catchClauses(),1);

	contents_->synchronizeLast(finallyOutline_, node()->finallyBody()->size() > 0, &style()->finallyOutline());
	if (finallyOutline_)
	{
		if (finallyOutline_->top() == nullptr) finallyOutline_->setTop(true);
		if (finallyOutline_->top()->first<Item>() == nullptr)
		{
			finallyIcon_ = new Static(finallyOutline_->top(), &style()->finallyIcon());
			// Note that if an old icon existed it would have been deleted when finallyOutline was deleted
			finallyOutline_->top()->setFirst(finallyIcon_);
		}
		if (finallyOutline_->content<Item>() == nullptr) finallyBody_ = nullptr;
		finallyOutline_->synchronizeContent(finallyBody_,node()->finallyBody(), &style()->finallyBody());
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	tryIcon_->setStyle(&style()->tryIcon());
	contents_->setStyle(&style()->contents());
	tryBody_->setStyle(&style()->tryBody());
	if (catchClauses_) catchClauses_->setStyle(&style()->catchClauses());
	if (finallyOutline_) finallyOutline_->setStyle(&style()->finallyOutline());
	if (finallyIcon_) finallyIcon_->setStyle(&style()->finallyIcon());
	if (finallyBody_) finallyBody_->setStyle(&style()->finallyBody());
}

} /* namespace OOVisualization */
