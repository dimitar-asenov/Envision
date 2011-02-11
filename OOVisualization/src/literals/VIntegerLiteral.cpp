/***********************************************************************************************************************
 * VIntegerLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VIntegerLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIntegerLiteral)

VIntegerLiteral::VIntegerLiteral(Item* parent, OOModel::IntegerLiteral *literal, const Visualization::TextStyle *style) :
	VInteger(parent, literal->valueNode(), style)
{
}

}
