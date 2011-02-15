/***********************************************************************************************************************
 * VUnaryOperation.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VUnaryOperation.h"

#include "OOModel/headers/expressions/UnaryOperation.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VUnaryOperation)

VUnaryOperation::VUnaryOperation(Item* parent, UnaryOperation* node, const OperatorSequenceStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->op(node->op()).container()) ),
	pre_( NULL ),
	post_( NULL ),
	expr_(NULL)
{
}

VUnaryOperation::~VUnaryOperation()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	pre_ = NULL;
	post_ = NULL;
	expr_ = NULL;
}

void VUnaryOperation::determineChildren()
{
	UnaryOperation* node = static_cast<UnaryOperation*> (getNode());

	if (!expr_)
	{
		expr_ = renderer()->render(NULL, node->operand());
		container_->append(expr_);
	}

	// Remove the prefix and postfix symbols if not there anymore
	if (pre_ && style()->op( node->op() ).preSymbol().symbol().isEmpty())
	{
		container_->remove(0);
		pre_ = NULL;
	}
	if (post_ && style()->op( node->op() ).postSymbol().symbol().isEmpty())
	{
		container_->remove(container_->length()-1);
		post_ = NULL;
	}

	// Rebuild the prefix and postfix symbols if needed
	if (!pre_ && !style()->op( node->op() ).preSymbol().symbol().isEmpty() )
	{
		pre_ = new Symbol(NULL, &style()->op( node->op() ).preSymbol());
		container_->prepend(pre_);
	}
	if (!post_ && !style()->op( node->op() ).postSymbol().symbol().isEmpty() )
	{
		post_ = new Symbol(NULL, &style()->op( node->op() ).postSymbol());
		container_->append(post_);
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->op(node->op()).container());
	if (pre_) pre_->setStyle( &style()->op(node->op()).preSymbol());
	if (post_) post_->setStyle( &style()->op(node->op()).postSymbol());
}

void VUnaryOperation::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VUnaryOperation::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
