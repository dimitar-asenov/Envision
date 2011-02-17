/***********************************************************************************************************************
 * VFloat.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VFloat.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VFloat)

VFloat::VFloat(Item* parent, NodeType *node, const StyleType *style) :
	ItemWithNode< TextRenderer, Model::Float >(parent, node, style)
{
	TextRenderer::setText( QString::number(node->get()) );
}

bool VFloat::setText(const QString& newText)
{
	bool ok = false;
	double value = newText.toDouble(&ok);
	if (ok)
	{
		node()->model()->beginModification(node(), "Set float");
		node()->set(value);
		node()->model()->endModification();
		TextRenderer::setText(newText);
		return true;
	}
	else return false;
}

}
