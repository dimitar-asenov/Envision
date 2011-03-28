/***********************************************************************************************************************
 * VIntegerLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VIntegerLiteral.h"

#include "ModelBase/headers/Model.h"

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIntegerLiteral, "item")

VIntegerLiteral::VIntegerLiteral(Item* parent, NodeType *literal, const StyleType *style) :
	ItemWithNode<Item, IntegerLiteral>(parent, literal, style),
	vis_(NULL)
{
}

VIntegerLiteral::~VIntegerLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VIntegerLiteral::determineChildren()
{
	synchronizeItem(vis_, node()->valueNode(), style());
	vis_->setStyle( style() );
}

void VIntegerLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VIntegerLiteral::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
