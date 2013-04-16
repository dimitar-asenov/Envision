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

#include "expressions/VCommaExpression.h"

#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VCommaExpression, "item")

VCommaExpression::VCommaExpression(Item* parent, NodeType* node, const StyleType* style) :
	BaseItemType(parent, node, style),
	pre_(nullptr),
	in_(nullptr),
	post_(nullptr),
	left_(nullptr),
	right_(nullptr)
{
}

VCommaExpression::~VCommaExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	pre_ = nullptr;
	in_ = nullptr;
	post_ = nullptr;
	left_ = nullptr;
	right_ = nullptr;
}

void VCommaExpression::determineChildren()
{
	const OperatorStyle* opStyle = &style()->op( 0 );

	int index = 0;

	layout()->synchronizeFirst(pre_ , !opStyle->preSymbol().isEmpty(), &opStyle->preSymbol());
	index += pre_?1:0;

	layout()->synchronizeMid(left_, node()->left(), index);
	index += left_?1:0;

	layout()->synchronizeMid(in_ , !opStyle->inSymbol().isEmpty(), &opStyle->inSymbol(), index);
	index += in_?1:0;

	layout()->synchronizeMid(right_, node()->right(), index);
	index += right_?1:0;

	layout()->synchronizeLast(post_ , !opStyle->postSymbol().isEmpty(), &opStyle->postSymbol());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &opStyle->layout());
	if (pre_) pre_->setStyle( &opStyle->preSymbol());
	if (in_) in_->setStyle( &opStyle->inSymbol());
	if (post_) post_->setStyle( &opStyle->postSymbol());
}

} /* namespace OOVisualization */
