/***********************************************************************************************************************
 * VIntegerLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VIntegerLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIntegerLiteral)

VIntegerLiteral::VIntegerLiteral(Item* parent, OOModel::IntegerLiteral *literal, const Visualization::TextStyle *style) :
	ModelItem(parent, literal, style), vis_(NULL)
{
}

VIntegerLiteral::~VIntegerLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VIntegerLiteral::determineChildren()
{
	OOModel::IntegerLiteral* literal = static_cast<OOModel::IntegerLiteral*> (getNode());
	if (!vis_) vis_ = renderer()->render(this, literal->valueNode());
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
