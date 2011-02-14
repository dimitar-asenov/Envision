/***********************************************************************************************************************
 * VNewExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VNewExpression.h"

#include "OOModel/headers/expressions/NewExpression.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VNewExpression)

VNewExpression::VNewExpression(Item* parent, NewExpression* node, const VNewExpressionStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	newSymbol_( new Symbol(NULL, &style->newSymbol()) ),
	type_(NULL),
	amountSymbol_( NULL ),
	amount_(NULL)
{
	container_->append(newSymbol_);
}

VNewExpression::~VNewExpression()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	newSymbol_ = NULL;
	amountSymbol_ = NULL;
	type_ = NULL;
	amount_ = NULL;
}

void VNewExpression::determineChildren()
{
	NewExpression* node = static_cast<NewExpression*> (getNode());

	if (!type_)
	{
		type_ = renderer()->render(NULL, node->type());
		container_->append(type_);
	}

	if (amount_ != NULL && amount_->getNode() != node->amount())
	{
		container_->remove(3);
		container_->remove(2);

		amount_ = NULL;
		amountSymbol_ = NULL;
	}

	if (amount_ == NULL && node->amount() )
	{
		amount_ = renderer()->render(NULL, node->amount());
		amountSymbol_ = new Symbol(NULL, &style()->amountSymbol());
		container_->append(amountSymbol_);
		container_->append(amount_);
	}


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->container());
	newSymbol_->setStyle( &style()->newSymbol());
	if (amountSymbol_) amountSymbol_->setStyle( &style()->amountSymbol());
}

void VNewExpression::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VNewExpression::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
