/***********************************************************************************************************************
 * VBooleanLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VBooleanLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VBooleanLiteral)

VBooleanLiteral::VBooleanLiteral(Item* parent, OOModel::BooleanLiteral *literal, const Visualization::TextStyle *style) :
	ModelItem(parent, literal, style), vis_(NULL)
{
}

VBooleanLiteral::~VBooleanLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VBooleanLiteral::determineChildren()
{
	OOModel::BooleanLiteral* literal = static_cast<OOModel::BooleanLiteral*> (getNode());
	if (!vis_) vis_ = renderer()->render(this, literal->valueNode());
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
