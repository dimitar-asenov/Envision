/***********************************************************************************************************************
 * VCharacterLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VCharacterLiteral.h"

#include "ModelBase/headers/Model.h"

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VCharacterLiteral)

VCharacterLiteral::VCharacterLiteral(Item* parent, NodeType* literal, const StyleType* style) :
	ItemWithNode<Item, CharacterLiteral>(parent, literal, style),
	vis_(NULL)
{
}

VCharacterLiteral::~VCharacterLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VCharacterLiteral::determineChildren()
{
	if (!vis_) vis_ = renderer()->render(this, node()->valueNode());
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
