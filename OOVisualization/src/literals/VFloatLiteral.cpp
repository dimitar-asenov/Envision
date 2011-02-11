/***********************************************************************************************************************
 * VFloatLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VFloatLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VFloatLiteral)

VFloatLiteral::VFloatLiteral(Item* parent, OOModel::FloatLiteral *literal, const Visualization::TextStyle *style) :
	VFloat(parent, literal->valueNode(), style)
{
}

}
