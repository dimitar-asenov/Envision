/***********************************************************************************************************************
 * VCharacterLiteral.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VCharacterLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VCharacterLiteral)

VCharacterLiteral::VCharacterLiteral(Item* parent, OOModel::CharacterLiteral *literal, const Visualization::TextStyle *style) :
	VCharacter(parent, literal->valueNode(), style)
{
}

}
