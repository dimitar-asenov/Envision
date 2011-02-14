/***********************************************************************************************************************
 * VBinaryOperation.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VBinaryOperation.h"

#include "OOModel/headers/expressions/BinaryOperation.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VBinaryOperation)

VBinaryOperation::VBinaryOperation(Item* parent, BinaryOperation* node, const OperatorSequenceStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->op(node->op()).container()) ),
	pre_( NULL ),
	in_( NULL ),
	post_( NULL ),
	left_(NULL),
	right_(NULL)
{
}

VBinaryOperation::~VBinaryOperation()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	pre_ = NULL;
	in_ = NULL;
	post_ = NULL;
	left_ = NULL;
	right_ = NULL;
}

void VBinaryOperation::determineChildren()
{
	BinaryOperation* node = static_cast<BinaryOperation*> (getNode());

	if (!left_)
	{
		left_ = renderer()->render(NULL, node->left());
		container_->append(left_);
		right_ = renderer()->render(NULL, node->right());
		container_->append(right_);
	}

	// Remove the prefix, infix and postfix symbols
	if (pre_)
	{
		container_->remove(0);
		pre_ = NULL;
	}
	if (in_)
	{
		container_->remove(1);
		in_ = NULL;
	}
	if (post_)
	{
		container_->remove(2);
		post_ = NULL;
	}

	// Rebuild the prefix, infix and postfix symbols
	if (! style()->op( node->op() ).inSymbol().symbol().isEmpty() )
	{
		in_ = new Symbol(NULL, &style()->op( node->op() ).inSymbol());
		container_->insert(in_, 1);
	}
	if (! style()->op( node->op() ).preSymbol().symbol().isEmpty() )
	{
		pre_ = new Symbol(NULL, &style()->op( node->op() ).preSymbol());
		container_->prepend(pre_);
	}
	if (! style()->op( node->op() ).postSymbol().symbol().isEmpty() )
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
	if (in_) in_->setStyle( &style()->op(node->op()).inSymbol());
	if (post_) post_->setStyle( &style()->op(node->op()).postSymbol());
}

void VBinaryOperation::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VBinaryOperation::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
