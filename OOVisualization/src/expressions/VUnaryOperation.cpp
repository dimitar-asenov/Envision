/***********************************************************************************************************************
 * VUnaryOperation.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VUnaryOperation.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VUnaryOperation, "item")

VUnaryOperation::VUnaryOperation(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, UnaryOperation>(parent, node, style),
	pre_( NULL ),
	post_( NULL ),
	expr_(NULL)
{
}

VUnaryOperation::~VUnaryOperation()
{
	// These were automatically deleted by LayoutProvider's destructor
	pre_ = NULL;
	post_ = NULL;
	expr_ = NULL;
}

void VUnaryOperation::determineChildren()
{
	const OperatorStyle* opStyle = &style()->op( node()->op() );

	int index = 0;
	layout()->synchronizeFirst(pre_ , !opStyle->preSymbol().isEmpty(), &opStyle->preSymbol());
	index += pre_?1:0;

	layout()->synchronizeMid(expr_, node()->operand(), index);
	index += expr_?1:0;

	layout()->synchronizeLast(post_ , !opStyle->postSymbol().isEmpty(), &opStyle->postSymbol());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->op(node()->op()).layout());
	if (pre_) pre_->setStyle( &style()->op(node()->op()).preSymbol());
	if (post_) post_->setStyle( &style()->op(node()->op()).postSymbol());
}

}
