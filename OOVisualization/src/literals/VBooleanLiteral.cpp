/***********************************************************************************************************************
 * VBooleanLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VBooleanLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VBooleanLiteral)

VBooleanLiteral::VBooleanLiteral(Item* parent, OOModel::BooleanLiteral *literal, const Visualization::TextStyle *style) :
	VBoolean(parent, literal->valueNode(), style)
{
}

}
