/***********************************************************************************************************************
 * VBooleanLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VBooleanLiteral.h"

#include "ModelBase/headers/Model.h"

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VBooleanLiteral)

VBooleanLiteral::VBooleanLiteral(Item* parent, NodeType *literal, const StyleType *style) :
	ItemWithNode< Item, BooleanLiteral>(parent, literal, style),
	vis_(NULL)
{
}

VBooleanLiteral::~VBooleanLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VBooleanLiteral::determineChildren()
{
	if (!vis_) vis_ = renderer()->render(this, node()->valueNode());
	vis_->setStyle( style() );
}

void VBooleanLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VBooleanLiteral::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
