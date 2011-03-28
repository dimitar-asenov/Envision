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
#include "ItemWithNode.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Character.h"

namespace Visualization {

class VISUALIZATIONBASE_API VCharacter : public ItemWithNode< TextRenderer, Model::Character >
{
	ITEM_COMMON_CUSTOM_STYLENAME(VCharacter, TextStyle)

	public:
		VCharacter(Item* parent, NodeType *node, const StyleType *style = itemStyles().get());
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VCHARACTER_H_ */
