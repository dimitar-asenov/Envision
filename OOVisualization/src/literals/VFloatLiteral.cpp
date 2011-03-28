/***********************************************************************************************************************
 * VFloatLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VFloatLiteral.h"

#include "ModelBase/headers/Model.h"

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VFloatLiteral, "item")

VFloatLiteral::VFloatLiteral(Item* parent, NodeType *node, const StyleType *style) :
	ItemWithNode<Item, FloatLiteral>(parent, node, style),
	vis_(NULL)
{
}

VFloatLiteral::~VFloatLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VFloatLiteral::determineChildren()
{
	synchronizeItem(vis_, node()->valueNode(), style());
	vis_->setStyle( style() );
}

void VFloatLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VFloatLiteral::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
