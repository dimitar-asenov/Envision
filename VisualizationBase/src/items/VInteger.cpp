/***********************************************************************************************************************
 * VInteger.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VInteger.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VInteger)

VInteger::VInteger(Item* parent, Model::Integer *integer, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, integer, style)
{
}

void VInteger::updateGeometry(int availableWidth, int availableHeight)
{
	Model::Integer* intNode = static_cast<Model::Integer*> (getNode());
	TextRenderer<ModelItem>::setText( QString::number(intNode->get()) );
	TextRenderer<ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
