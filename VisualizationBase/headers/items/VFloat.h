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
#include "ItemWithNode.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Float.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VFloat : public ItemWithNode<TextRenderer, Model::Float>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VFloat, TextStyle)

	public:
		VFloat(Item* parent, NodeType *node, const StyleType *style = Styles::item<VFloat>("default"));
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VFLOAT_H_ */
