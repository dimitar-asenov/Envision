/***********************************************************************************************************************
 * VBoolean.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBoolean.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VBoolean, "item")

VBoolean::VBoolean(Item* parent, NodeType *node, const StyleType *style) :
	ItemWithNode< TextRenderer, Model::Boolean >(parent, node, style)
{
	if (node->get()) TextRenderer::setText( "true" );
	else TextRenderer::setText( "false" );
}

bool VBoolean::setText(const QString& newText)
{
	if (newText.contains(' ') || newText.compare("true", Qt::CaseInsensitive) == 0 || newText.compare("false", Qt::CaseInsensitive) == 0)
	{
		node()->model()->beginModification(node(), "Set boolean");
		if ( newText.contains(' ') ) node()->set( !node()->get() );
		else if ( newText.compare("true", Qt::CaseInsensitive) == 0 ) node()->set(true);
		else node()->set(false);
		node()->model()->endModification();
		if (node()->get()) TextRenderer::setText( "true" );
		else TextRenderer::setText( "false" );
		return true;
	}
	else return false;
}

QString VBoolean::currentText()
{
	return node()->get() ? "true" : "false" ;
}

}
