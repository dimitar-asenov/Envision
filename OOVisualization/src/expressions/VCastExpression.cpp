/***********************************************************************************************************************
 * VCastExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VCastExpression.h"

#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VCastExpression, "item")

VCastExpression::VCastExpression(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, CastExpression>(parent, node, style),
	type_(NULL),
	expr_(NULL)
{
}

VCastExpression::~VCastExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	type_ = NULL;
	expr_ = NULL;
}

void VCastExpression::determineChildren()
{
	layout()->synchronizeFirst(expr_, node()->expr());
	layout()->synchronizeLast(type_, node()->type());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
}

}
