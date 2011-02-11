/***********************************************************************************************************************
 * VFloatLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VFloatLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VFloatLiteral)

VFloatLiteral::VFloatLiteral(Item* parent, OOModel::FloatLiteral *literal, const Visualization::TextStyle *style) :
	ModelItem(parent, literal, style), vis_(NULL)
{
}

VFloatLiteral::~VFloatLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VFloatLiteral::determineChildren()
{
	OOModel::FloatLiteral* literal = static_cast<OOModel::FloatLiteral*> (getNode());
	if (!vis_) vis_ = renderer()->render(this, literal->valueNode());
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
