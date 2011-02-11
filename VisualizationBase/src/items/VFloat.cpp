/***********************************************************************************************************************
 * VFloat.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VFloat.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VFloat)

VFloat::VFloat(Item* parent, Model::Float *node, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, node, style)
{
}

void VFloat::updateGeometry(int availableWidth, int availableHeight)
{
	Model::Float* node = static_cast<Model::Float*> (getNode());
	TextRenderer<ModelItem>::setText( QString::number(node->get()) );
	TextRenderer<ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
