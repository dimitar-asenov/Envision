/***********************************************************************************************************************
 * VCastExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VCastExpression.h"

#include "OOModel/headers/expressions/CastExpression.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VCastExpression)

VCastExpression::VCastExpression(Item* parent, CastExpression* node, const VCastExpressionStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	separator_( new Symbol(NULL, &style->separator())),
	type_(NULL),
	expr_(NULL)
{
	container_->append(separator_);
}

VCastExpression::~VCastExpression()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	separator_ = NULL;
	type_ = NULL;
	expr_ = NULL;
}

void VCastExpression::determineChildren()
{
	if (!type_)
	{
		CastExpression* node = static_cast<CastExpression*> (getNode());
		type_ = renderer()->render(NULL, node->type());
		expr_ = renderer()->render(NULL, node->expr());
		container_->prepend(type_);
		container_->append(expr_);
	}


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->container());
	separator_->setStyle( &style()->separator());
}

void VCastExpression::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VCastExpression::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
