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
#include "ItemWithNode.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Integer.h"

namespace Visualization {

class VISUALIZATIONBASE_API VInteger : public ItemWithNode<TextRenderer, Model::Integer>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VInteger, TextStyle)

	public:
		VInteger(Item* parent, NodeType *node, const StyleType *style = itemStyles().get());
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VINTEGER_H_ */
