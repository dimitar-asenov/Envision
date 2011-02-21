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

VCharacter::VCharacter(Item* parent, NodeType *node, const StyleType *style) :
	ItemWithNode< TextRenderer, Model::Character >(parent, node, style)
{
	TextRenderer::setText( node->get() );
}

bool VCharacter::setText(const QString& newText)
{
	if (newText.length() == 1)
	{
		node()->model()->beginModification(node(), "Set character");
		node()->set(newText[0]);
		node()->model()->endModification();
		TextRenderer::setText(newText[0]);
		return true;
	}
	else return false;
}

QString VCharacter::currentText()
{
	return node()->get();
}


}
