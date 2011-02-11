/***********************************************************************************************************************
 * VBoolean.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBoolean.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VBoolean)

VBoolean::VBoolean(Item* parent, Model::Boolean *node, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, node, style)
{
}

void VBoolean::updateGeometry(int availableWidth, int availableHeight)
{
	Model::Boolean* node = static_cast<Model::Boolean*> (getNode());
	if (node->get() == true) TextRenderer<ModelItem>::setText( "true");
	else TextRenderer<ModelItem>::setText( "false" );
	TextRenderer<ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
