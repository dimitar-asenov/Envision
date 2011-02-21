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
#include "ItemWithNode.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Boolean.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VBoolean : public ItemWithNode<TextRenderer, Model::Boolean>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBoolean, TextStyle)

	public:
		VBoolean(Item* parent, NodeType *node, const StyleType *style = Styles::item<VBoolean>("default"));
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VBOOLEAN_H_ */
