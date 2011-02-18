/***********************************************************************************************************************
 * VCastExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VCastExpression.h"

#include "OOModel/headers/expressions/CastExpression.h"

#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VCastExpression)

VCastExpression::VCastExpression(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, CastExpression>(parent, node, style),
	separator_( new Symbol(NULL, &style->separator())),
	type_(NULL),
	expr_(NULL)
{
	layout()->append(separator_);
}

VCastExpression::~VCastExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	separator_ = NULL;
	type_ = NULL;
	expr_ = NULL;
}

void VCastExpression::determineChildren()
{
	if (!type_)
	{
		type_ = renderer()->render(NULL, node()->type());
		expr_ = renderer()->render(NULL, node()->expr());
		layout()->prepend(type_);
		layout()->append(expr_);
	}


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->container());
	separator_->setStyle( &style()->separator());
}

}
