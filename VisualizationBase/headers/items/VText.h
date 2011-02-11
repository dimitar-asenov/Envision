/***********************************************************************************************************************
 * VText.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VTEXT_H_
#define VTEXT_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"
#include "ModelItem.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Text.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VText : public TextRenderer<ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VText, TextRenderer<ModelItem>, TextStyle)

	public:
		VText(Item* parent, Model::Text *text, const TextStyle *style = Styles::item<VText>("default"));
		virtual bool setText(const QString& newText);
};

}

#endif /* VTEXT_H_ */
