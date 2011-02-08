/***********************************************************************************************************************
 * VStringLiteral.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VStringLiteral.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStringLiteral)

VStringLiteral::VStringLiteral(Item* parent, OOModel::StringLiteral *literal, const Visualization::TextStyle *style) :
	Visualization::TextRenderer<Visualization::ModelItem>(parent, literal, style)
{
}

void VStringLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	OOModel::StringLiteral* literal = static_cast<OOModel::StringLiteral*> (getNode());
	setText(literal->value());
	Visualization::TextRenderer<Visualization::ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
