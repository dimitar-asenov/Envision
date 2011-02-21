/***********************************************************************************************************************
 * VReference.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VREFERENCE_H_
#define VREFERENCE_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"
#include "ItemWithNode.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Reference.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VReference : public ItemWithNode<TextRenderer, Model::Reference>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VReference, TextStyle)

	public:
		VReference(Item* parent, NodeType *node, const StyleType *style = Styles::item<VReference>("default"));
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VREFERENCE_H_ */
