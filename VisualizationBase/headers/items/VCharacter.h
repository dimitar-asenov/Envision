/***********************************************************************************************************************
 * VCharacter.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCHARACTER_H_
#define VCHARACTER_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"
#include "ModelItem.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Character.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VCharacter : public TextRenderer<ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VCharacter, TextRenderer<ModelItem>, TextStyle)

	public:
		VCharacter(Item* parent, Model::Character *node, const TextStyle *style = Styles::item<VCharacter>("default"));
		virtual void setText(const QString& newText);
};

}

#endif /* VCHARACTER_H_ */
