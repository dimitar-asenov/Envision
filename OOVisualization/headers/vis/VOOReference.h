/***********************************************************************************************************************
 * VOOReference.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VOOREFERENCE_H_
#define VOOREFERENCE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/OOReference.h"

namespace OOVisualization {

class OOVISUALIZATION_API VOOReference : public Visualization::TextRenderer<Visualization::ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VOOReference, Visualization::TextRenderer<Visualization::ModelItem>, Visualization::TextStyle)

	public:
		VOOReference(Item* parent, OOModel::OOReference *ref, const Visualization::TextStyle *style = Visualization::Styles::item<VOOReference>("default"));

		virtual void updateGeometry(int availableWidth, int availableHeight);
};

}

#endif /* VOOREFERENCE_H_ */
