/***********************************************************************************************************************
 * VField.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFIELD_H_
#define VFIELD_H_

#include "../oovisualization_api.h"
#include "VFieldStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
}

namespace OOModel {
	class Field;
}

namespace OOVisualization {

class OOVISUALIZATION_API VField : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::Field >
{
	ITEM_COMMON(VField)

	public:
		VField(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VField>("default"));
		virtual ~VField();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();

	private:
		Visualization::VText* name_;
		Visualization::Item* type_;
};
}

#endif /* VFIELD_H_ */
