/***********************************************************************************************************************
 * VReference.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VReference.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VReference, "item")

VReference::VReference(Item* parent, NodeType *node, const StyleType *style) :
	ItemWithNode<TextRenderer, Model::Reference>(parent, node, style)
{
}

bool VReference::setText(const QString& newText)
{
	// TODO: Check if this is a valid path?
	node()->model()->beginModification(node(), "Set boolean");
	node()->set(newText);
	node()->model()->endModification();
	return true;
}

QString VReference::currentText()
{
	return node()->path();
}

}
