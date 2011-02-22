/***********************************************************************************************************************
 * VStringLiteral.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VStringLiteral.h"

#include "ModelBase/headers/Model.h"

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStringLiteral)

VStringLiteral::VStringLiteral(Item* parent, NodeType* literal, const StyleType* style) :
	ItemWithNode<Item, StringLiteral>(parent, literal, style),
	vis_(NULL)
{
}

VStringLiteral::~VStringLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VStringLiteral::determineChildren()
{
	synchronizeItem(vis_, node()->valueNode(), style());
	vis_->setStyle( style() );
}

void VStringLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VStringLiteral::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
