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

#include "OOModel/headers/top_level/Field.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class VText;
}

namespace OOVisualization {

class OOVISUALIZATION_API VField : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::Field >
{
	ITEM_COMMON(VField)

	public:
		VField(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VField();

	protected:
		void determineChildren();

	private:
		Visualization::VText* name_;
		Visualization::Item* type_;
};
}

#endif /* VFIELD_H_ */
