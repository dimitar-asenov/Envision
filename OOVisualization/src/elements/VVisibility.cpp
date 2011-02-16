/***********************************************************************************************************************
 * VVisibility.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VVisibility.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VVisibility)

VVisibility::VVisibility(Item* parent, OOModel::Visibility *vis, const Visualization::TextStyle *style) :
	Visualization::TextRenderer<Visualization::ModelItem>(parent, vis, style)
{
}

void VVisibility::updateGeometry(int availableWidth, int availableHeight)
{
	OOModel::Visibility* visNode = static_cast<OOModel::Visibility*> (getNode());
	switch (visNode->get())
	{
		case OOModel::Visibility::DEFAULT:
			setText( "" );
			break;
		case OOModel::Visibility::PUBLIC:
			setText( "public" );
			break;
		case OOModel::Visibility::PRIVATE:
			setText( "private" );
			break;
		case OOModel::Visibility::PROTECTED:
			setText( "protected" );
			break;
		default:
			setText( "unknown visibility" );
			break;
	}
	Visualization::TextRenderer<Visualization::ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
