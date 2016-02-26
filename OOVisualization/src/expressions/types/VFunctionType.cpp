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

#include "VFunctionType.h"

#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VFunctionType, "item")

VFunctionType::VFunctionType(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{
	icon_ = new Static{layout(), &style->icon()};
	layout()->append(icon_);
}

VFunctionType::~VFunctionType()
{
	// These were automatically deleted by LayoutProvider's destructor
	icon_ = nullptr;
	arguments_ = nullptr;
	resultSymbol_ = nullptr;
	results_ = nullptr;

}

void VFunctionType::determineChildren()
{
	layout()->synchronizeFirst(icon_, true, &style()->icon());
	layout()->synchronizeMid(arguments_, node()->arguments(), &style()->arguments(), 1);
	layout()->synchronizeLast(results_, node()->results()->size() ? node()->results() : nullptr, &style()->results());
	layout()->synchronizeMid(resultSymbol_, results_, &style()->resultSymbol(), 2);


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	icon_->setStyle( &style()->icon());
	arguments_->setStyle( &style()->arguments() );
	if (results_)
	{
		resultSymbol_->setStyle( &style()->resultSymbol());
		results_->setStyle( &style()->results() );
	}
}

}
