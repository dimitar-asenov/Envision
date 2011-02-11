/***********************************************************************************************************************
 * VCharacterLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCHARACTERLITERAL_H_
#define VCHARACTERLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/VCharacter.h"

#include "OOModel/headers/expressions/CharacterLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VCharacterLiteral : public Visualization::VCharacter
{
	ITEM_COMMON_CUSTOM_STYLENAME(VCharacterLiteral, Visualization::VCharacter, Visualization::TextStyle)

	public:
		VCharacterLiteral(Item* parent, OOModel::CharacterLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VCharacterLiteral>("default"));
};

}

#endif /* VCHARACTERLITERAL_H_ */
