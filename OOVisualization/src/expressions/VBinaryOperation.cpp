/***********************************************************************************************************************
 * VBinaryOperation.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VBinaryOperation.h"

#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VBinaryOperation)

VBinaryOperation::VBinaryOperation(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, BinaryOperation>(parent, node, style),
	pre_( NULL ),
	in_( NULL ),
	post_( NULL ),
	left_(NULL),
	right_(NULL)
{
}

VBinaryOperation::~VBinaryOperation()
{
	// These were automatically deleted by LayoutProvider's destructor
	pre_ = NULL;
	in_ = NULL;
	post_ = NULL;
	left_ = NULL;
	right_ = NULL;
}

void VBinaryOperation::determineChildren()
{
	if (!left_)
	{
		left_ = renderer()->render(NULL, node()->left());
		layout()->append(left_);
		right_ = renderer()->render(NULL, node()->right());
		layout()->append(right_);
	}

	// Remove the prefix, infix and postfix symbols if they are not there any more
	if (in_ && style()->op( node()->op() ).inSymbol().symbol().isEmpty() )
	{
		layout()->removeAll( in_ );
		in_ = NULL;
	}
	if (pre_ && style()->op( node()->op() ).preSymbol().symbol().isEmpty())
	{
		layout()->removeAll( pre_ );
		pre_ = NULL;
	}
	if (post_ && style()->op( node()->op() ).postSymbol().symbol().isEmpty() )
	{
		layout()->removeAll( post_ );
		post_ = NULL;
	}

	// Rebuild the prefix, infix and postfix symbols if needed
	if (!in_ && !style()->op( node()->op() ).inSymbol().symbol().isEmpty() )
	{
		in_ = new Symbol(NULL, &style()->op( node()->op() ).inSymbol());
		layout()->insert(in_, (pre_?2:1) );
	}
	if (!pre_ && !style()->op( node()->op() ).preSymbol().symbol().isEmpty() )
	{
		pre_ = new Symbol(NULL, &style()->op( node()->op() ).preSymbol());
		layout()->prepend(pre_);
	}
	if (!post_ && !style()->op( node()->op() ).postSymbol().symbol().isEmpty() )
	{
		post_ = new Symbol(NULL, &style()->op( node()->op() ).postSymbol());
		layout()->append(post_);
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->op(node()->op()).layout());
	if (pre_) pre_->setStyle( &style()->op(node()->op()).preSymbol());
	if (in_) in_->setStyle( &style()->op(node()->op()).inSymbol());
	if (post_) post_->setStyle( &style()->op(node()->op()).postSymbol());
}

}
