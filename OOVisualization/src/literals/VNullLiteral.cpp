/***********************************************************************************************************************
 * VNullLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VNullLiteral.h"

#include "OOModel/headers/expressions/NullLiteral.h"

#include "VisualizationBase/headers/items/Text.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VNullLiteral)

VNullLiteral::VNullLiteral(Item* parent, NullLiteral* node, const SymbolStyle* style) :
	ModelItem(parent, node, style),
	vis_(new Symbol(this, style))
{
}

VNullLiteral::~VNullLiteral()
{
	SAFE_DELETE_ITEM(vis_);
}

void VNullLiteral::determineChildren()
{
	vis_->setStyle( style());
}

void VNullLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VNullLiteral::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
