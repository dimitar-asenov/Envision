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
#include "ItemWithNode.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Text.h"

namespace Visualization {

class VISUALIZATIONBASE_API VText : public ItemWithNode<TextRenderer, Model::Text>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VText, TextStyle)

	public:
		VText(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VTEXT_H_ */
