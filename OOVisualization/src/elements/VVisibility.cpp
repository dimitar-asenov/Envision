/***********************************************************************************************************************
 * VVisibility.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VVisibility.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VVisibility)

VVisibility::VVisibility(Item* parent, NodeType* vis, const StyleType* style) :
	ItemWithNode<TextRenderer, Visibility>(parent, vis, style)
{
}

void VVisibility::updateGeometry(int availableWidth, int availableHeight)
{
	switch (node()->get())
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
	TextRenderer::updateGeometry(availableWidth, availableHeight);
}

}
