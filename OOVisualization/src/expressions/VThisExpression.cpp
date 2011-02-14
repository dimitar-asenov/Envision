/***********************************************************************************************************************
 * VThisExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VThisExpression.h"

#include "OOModel/headers/expressions/ThisExpression.h"

#include "VisualizationBase/headers/items/Text.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VThisExpression)

VThisExpression::VThisExpression(Item* parent, ThisExpression* node, const SymbolStyle* style) :
	ModelItem(parent, node, style),
	vis_(new Symbol(this, style))
{
}

VThisExpression::~VThisExpression()
{
	SAFE_DELETE_ITEM(vis_);
}

void VThisExpression::determineChildren()
{
	vis_->setStyle( style());
}

void VThisExpression::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VThisExpression::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
