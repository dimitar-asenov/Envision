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

VFloat::VFloat(Item* parent, Model::Float *node, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, node, style)
{
	TextRenderer<ModelItem>::setText( QString::number(node->get()) );
}

void VFloat::setText(const QString& newText)
{
	bool ok = false;
	double value = newText.toDouble(&ok);
	if (ok)
	{
		Model::Float* node = static_cast<Model::Float*> (getNode());
		node->model()->beginModification(node, "Set float");
		node->set(value);
		node->model()->endModification();
		TextRenderer<ModelItem>::setText(newText);
	}
}

}
