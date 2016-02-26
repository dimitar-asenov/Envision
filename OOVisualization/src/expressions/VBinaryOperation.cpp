/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "VBinaryOperation.h"

#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VBinaryOperation, "item")

VBinaryOperation::VBinaryOperation(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style},
	pre_{nullptr},
	in_{nullptr},
	post_{nullptr},
	left_{nullptr},
	right_{nullptr}
{
}

VBinaryOperation::~VBinaryOperation()
{
	// These were automatically deleted by LayoutProvider's destructor
	pre_ = nullptr;
	in_ = nullptr;
	post_ = nullptr;
	left_ = nullptr;
	right_ = nullptr;
}

void VBinaryOperation::determineChildren()
{
	const OperatorStyle* opStyle = &style()->op( node()->op() );

	int index = 0;

	layout()->synchronizeFirst(pre_, !opStyle->preSymbol().isEmpty(), &opStyle->preSymbol());
	index += pre_?1:0;

	layout()->synchronizeMid(left_, node()->left(), index);
	index += left_?1:0;

	layout()->synchronizeMid(in_, !opStyle->inSymbol().isEmpty(), &opStyle->inSymbol(), index);
	index += in_?1:0;

	layout()->synchronizeMid(right_, node()->right(), index);
	index += right_?1:0;

	layout()->synchronizeLast(post_, !opStyle->postSymbol().isEmpty(), &opStyle->postSymbol());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &opStyle->layout());
	if (pre_) pre_->setStyle( &opStyle->preSymbol());
	if (in_) in_->setStyle( &opStyle->inSymbol());
	if (post_) post_->setStyle( &opStyle->postSymbol());

	// Set the spacing of this layout
	int depth = getExpressionDepth(node());
	if (depth <= 1) layout()->setSpaceBetweenElements(false, 0);
	else
	{
		int space = 1;
		for (int i = 0; i<depth; ++i) space *= 2;
		layout()->setSpaceBetweenElements(true, space + opStyle->layout().spaceBetweenElements());
	}
}


int VBinaryOperation::getExpressionDepth(OOModel::Expression* e, int* op) const
{
	auto binary = DCast<OOModel::BinaryOperation>(e);
	if (!binary) return 0;
	auto op_id = binary->op();
	if (op) *op = op_id;

	int op_ida = -1;
	int op_idb = -1;
	int a = getExpressionDepth(binary->left(), &op_ida);
	int b = getExpressionDepth(binary->right(), &op_idb);

	if (op_ida == op_idb && op_ida == op_id) return a>b?a:b;
	if (op_ida == op_id) return a;
	if (op_idb == op_id) return b;

	return a>b ? a+1 : b+1;
}

}
