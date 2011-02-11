/***********************************************************************************************************************
 * VBoolean.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBoolean.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VBoolean)

VBoolean::VBoolean(Item* parent, Model::Boolean *node, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, node, style)
{
	if (node->get()) TextRenderer<ModelItem>::setText( "true" );
	else TextRenderer<ModelItem>::setText( "false" );
}

void VBoolean::setText(const QString& newText)
{
	if (newText.contains(' ') || newText.compare("true", Qt::CaseInsensitive) == 0 || newText.compare("false", Qt::CaseInsensitive) == 0)
	{
		Model::Boolean* node = static_cast<Model::Boolean*> (getNode());
		node->model()->beginModification(node, "Set boolean");
		if ( newText.contains(' ') ) node->set( !node->get() );
		else if ( newText.compare("true", Qt::CaseInsensitive) == 0 ) node->set(true);
		else node->set(false);
		node->model()->endModification();
		TextRenderer<ModelItem>::setText(newText.toLower());
		TextRenderer<ModelItem>::setCaretPosition(0);
	}
}

}
