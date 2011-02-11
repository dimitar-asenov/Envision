/***********************************************************************************************************************
 * VFloat.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFLOAT_H_
#define VFLOAT_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"
#include "ModelItem.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Float.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VFloat : public TextRenderer<ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VFloat, TextRenderer<ModelItem>, TextStyle)

	public:
		VFloat(Item* parent, Model::Float *node, const TextStyle *style = Styles::item<VFloat>("default"));
		virtual bool setText(const QString& newText);
};

}

#endif /* VFLOAT_H_ */
