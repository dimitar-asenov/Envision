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

VInteger::VInteger(Item* parent, Model::Integer *integer, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, integer, style)
{
	TextRenderer<ModelItem>::setText( QString::number(integer->get()) );
}

void VInteger::setText(const QString& newText)
{
	bool ok = false;
	int value = newText.toInt(&ok);
	if (ok)
	{
		Model::Integer* node = static_cast<Model::Integer*> (getNode());
		node->model()->beginModification(node, "Set integer");
		node->set(value);
		node->model()->endModification();
		TextRenderer<ModelItem>::setText(newText);
	}
}

}
