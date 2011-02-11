/***********************************************************************************************************************
 * VCharacter.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VCharacter.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VCharacter)

VCharacter::VCharacter(Item* parent, Model::Character *node, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, node, style)
{
	TextRenderer<ModelItem>::setText( node->get() );
}

bool VCharacter::setText(const QString& newText)
{
	if (newText.length() == 1)
	{
		Model::Character* node = static_cast<Model::Character*> (getNode());
		node->model()->beginModification(node, "Set character");
		node->set(newText[0]);
		node->model()->endModification();
		TextRenderer<ModelItem>::setText(newText[0]);
		return true;
	}
	else return false;
}

}
