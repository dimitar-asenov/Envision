/***********************************************************************************************************************
 * VStatic.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTATIC_H_
#define VSTATIC_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/elements/Static.h"

namespace OOVisualization {

class OOVISUALIZATION_API VStatic : public Visualization::TextRenderer<Visualization::ModelItem>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VStatic, Visualization::TextStyle)

	public:
		VStatic(Item* parent, OOModel::Static *stat, const Visualization::TextStyle *style = Visualization::Styles::item<VStatic>("default"));

		virtual void updateGeometry(int availableWidth, int availableHeight);
};

}

#endif /* VSTATIC_H_ */
