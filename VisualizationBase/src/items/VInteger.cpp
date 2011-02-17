/***********************************************************************************************************************
 * VInteger.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VInteger.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VInteger)

VInteger::VInteger(Item* parent, NodeType *node, const StyleType *style) :
	ItemWithNode<TextRenderer, Model::Integer>(parent, node, style)
{
	TextRenderer::setText( QString::number(node->get()) );
}

bool VInteger::setText(const QString& newText)
{
	bool ok = false;
	int value = newText.toInt(&ok);
	if (ok)
	{
		node()->model()->beginModification(node(), "Set integer");
		node()->set(value);
		node()->model()->endModification();
		TextRenderer::setText(newText);
		return true;
	}
	else return false;
}

}
