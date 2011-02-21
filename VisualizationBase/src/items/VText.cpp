/***********************************************************************************************************************
 * VText.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VText.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VText)

VText::VText(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<TextRenderer, Model::Text>(parent, node, style)
{
	TextRenderer::setText(node->get());
}

bool VText::setText(const QString& newText)
{
	node()->model()->beginModification(node(), "Set text");
	node()->set(newText);
	node()->model()->endModification();
	return TextRenderer::setText(newText);
}

QString VText::currentText()
{
	return node()->get();
}

}
