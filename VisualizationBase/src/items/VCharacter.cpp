/***********************************************************************************************************************
 * VCharacter.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VCharacter.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VCharacter)

VCharacter::VCharacter(Item* parent, Model::Character *node, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, node, style)
{
}

void VCharacter::updateGeometry(int availableWidth, int availableHeight)
{
	Model::Character* node = static_cast<Model::Character*> (getNode());
	TextRenderer<ModelItem>::setText( node->get() );
	TextRenderer<ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
