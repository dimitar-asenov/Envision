/***********************************************************************************************************************
 * VStatic.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VStatic.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStatic)

VStatic::VStatic(Item* parent, OOModel::Static *stat, const Visualization::TextStyle *style) :
	Visualization::TextRenderer<Visualization::ModelItem>(parent, stat, style)
{
}

void VStatic::updateGeometry(int availableWidth, int availableHeight)
{
	OOModel::Static* statNode = static_cast<OOModel::Static*> (getNode());
	switch (statNode->get())
	{
		case OOModel::Static::CLASS_VARIABLE:
			setText( "static" );
			break;
		case OOModel::Static::INSTANCE_VARIABLE:
			setText( "no" );
			break;
		default:
			setText( "unknown static" );
			break;
	}
	Visualization::TextRenderer<Visualization::ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
