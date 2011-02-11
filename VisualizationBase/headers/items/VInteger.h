/***********************************************************************************************************************
 * VInteger.h
 *
 *  Created on: Feb 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VINTEGER_H_
#define VINTEGER_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"
#include "ModelItem.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Integer.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VInteger : public TextRenderer<ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VInteger, TextRenderer<ModelItem>, TextStyle)

	public:
		VInteger(Item* parent, Model::Integer *integer, const TextStyle *style = Styles::item<VInteger>("default"));
		virtual bool setText(const QString& newText);
};

}

#endif /* VINTEGER_H_ */
