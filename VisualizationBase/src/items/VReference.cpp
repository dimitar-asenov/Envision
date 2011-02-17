/***********************************************************************************************************************
 * VReference.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VReference.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VReference)

VReference::VReference(Item* parent, NodeType *node, const StyleType *style) :
	ItemWithNode<TextRenderer, Model::Reference>(parent, node, style)
{
}

void VReference::updateGeometry(int availableWidth, int availableHeight)
{
	setText( node()->path() );
	TextRenderer::updateGeometry(availableWidth, availableHeight);
}

}
