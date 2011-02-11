/***********************************************************************************************************************
 * VBoolean.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBOOLEAN_H_
#define VBOOLEAN_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"
#include "ModelItem.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Boolean.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VBoolean : public TextRenderer<ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBoolean, TextRenderer<ModelItem>, TextStyle)

	public:
		VBoolean(Item* parent, Model::Boolean *node, const TextStyle *style = Styles::item<VBoolean>("default"));
		virtual void setText(const QString& newText);
};

}

#endif /* VBOOLEAN_H_ */
