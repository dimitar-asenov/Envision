/***********************************************************************************************************************
 * VReference.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VReference.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VReference)

VReference::VReference(Item* parent, Model::Reference *ref, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, ref, style)
{
}

void VReference::updateGeometry(int availableWidth, int availableHeight)
{
	Model::Reference* refNode = static_cast<Model::Reference*> (getNode());
	setText( refNode->path() );
	TextRenderer<ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
