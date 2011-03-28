/***********************************************************************************************************************
 * VNewExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VNewExpression.h"

#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VNewExpression, "item")

VNewExpression::VNewExpression(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, NewExpression>(parent, node, style),
	newSymbol_( new Symbol(NULL, &style->newSymbol()) ),
	type_(NULL),
	amount_(NULL)
{
	layout()->append(newSymbol_);
}

VNewExpression::~VNewExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	newSymbol_ = NULL;
	type_ = NULL;
	amount_ = NULL;
}

void VNewExpression::determineChildren()
{
	layout()->synchronizeFirst(amount_, node()->amount());
	layout()->synchronizeLast(type_, node()->type());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	newSymbol_->setStyle( &style()->newSymbol());
}

}
