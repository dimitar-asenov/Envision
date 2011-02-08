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
#include "ModelItem.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Reference.h"
#include "../Styles.h"

namespace Visualization {

class VISUALIZATIONBASE_API VReference : public TextRenderer<ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VReference, TextRenderer<ModelItem>, TextStyle)

	public:
		VReference(Item* parent, Model::Reference *ref, const TextStyle *style = Styles::item<VReference>("default"));

		virtual void updateGeometry(int availableWidth, int availableHeight);
};

}

#endif /* VREFERENCE_H_ */
