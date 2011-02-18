/***********************************************************************************************************************
 * VNewExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VNewExpression.h"

#include "OOModel/headers/expressions/NewExpression.h"

#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VNewExpression)

VNewExpression::VNewExpression(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, NewExpression>(parent, node, style),
	newSymbol_( new Symbol(NULL, &style->newSymbol()) ),
	type_(NULL),
	amountSymbol_( NULL ),
	amount_(NULL)
{
	layout()->append(newSymbol_);
}

VNewExpression::~VNewExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	newSymbol_ = NULL;
	amountSymbol_ = NULL;
	type_ = NULL;
	amount_ = NULL;
}

void VNewExpression::determineChildren()
{
	if (!type_)
	{
		type_ = renderer()->render(NULL, node()->type());
		layout()->append(type_);
	}

	if (amount_ != NULL && amount_->node() != node()->amount())
	{
		layout()->remove(3);
		layout()->remove(2);

		amount_ = NULL;
		amountSymbol_ = NULL;
	}

	if (amount_ == NULL && node()->amount() )
	{
		amount_ = renderer()->render(NULL, node()->amount());
		amountSymbol_ = new Symbol(NULL, &style()->amountSymbol());
		layout()->append(amountSymbol_);
		layout()->append(amount_);
	}


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->container());
	newSymbol_->setStyle( &style()->newSymbol());
	if (amountSymbol_) amountSymbol_->setStyle( &style()->amountSymbol());
}

}
