/***********************************************************************************************************************
 * VVisibility.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VVISIBILITY_H_
#define VVISIBILITY_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/elements/Visibility.h"

namespace OOVisualization {

class OOVISUALIZATION_API VVisibility : public Visualization::ItemWithNode< Visualization::TextRenderer, OOModel::Visibility>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VVisibility, Visualization::TextStyle)

	public:
		VVisibility(Item* parent, NodeType* vis, const StyleType* style = Visualization::Styles::item<VVisibility>("default"));
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VVISIBILITY_H_ */
