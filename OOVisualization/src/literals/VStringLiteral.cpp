/***********************************************************************************************************************
 * VStringLiteral.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VStringLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStringLiteral)

VStringLiteral::VStringLiteral(Item* parent, OOModel::StringLiteral *literal, const Visualization::TextStyle *style) :
	ModelItem(parent, literal, style), vis_(NULL)
{
}

VStringLiteral::~VStringLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VStringLiteral::determineChildren()
{
	OOModel::StringLiteral* literal = static_cast<OOModel::StringLiteral*> (getNode());
	if (!vis_) vis_ = renderer()->render(this, literal->valueNode());
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
