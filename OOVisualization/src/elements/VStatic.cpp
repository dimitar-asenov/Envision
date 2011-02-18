/***********************************************************************************************************************
 * VStatic.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VStatic.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStatic)

VStatic::VStatic(Item* parent, NodeType* stat, const StyleType* style) :
	ItemWithNode<TextRenderer, Static>(parent, stat, style)
{
}

void VStatic::updateGeometry(int availableWidth, int availableHeight)
{
	switch (node()->get())
	{
		case OOModel::Static::CLASS_VARIABLE:
			setText( "static" );
			break;
		case OOModel::Static::INSTANCE_VARIABLE:
			setText( "not static" );
			break;
		default:
			setText( "unknown static" );
			break;
	}
	TextRenderer::updateGeometry(availableWidth, availableHeight);
}

}
