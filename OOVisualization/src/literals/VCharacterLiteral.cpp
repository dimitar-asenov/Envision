/***********************************************************************************************************************
 * VCharacterLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VCharacterLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VCharacterLiteral)

VCharacterLiteral::VCharacterLiteral(Item* parent, OOModel::CharacterLiteral *literal, const Visualization::TextStyle *style) :
	ModelItem(parent, literal, style), vis_(NULL)
{
}

VCharacterLiteral::~VCharacterLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VCharacterLiteral::determineChildren()
{
	OOModel::CharacterLiteral* literal = static_cast<OOModel::CharacterLiteral*> (getNode());
	if (!vis_) vis_ = renderer()->render(this, literal->valueNode());
	vis_->setStyle( style() );
}

void VCharacterLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VCharacterLiteral::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
