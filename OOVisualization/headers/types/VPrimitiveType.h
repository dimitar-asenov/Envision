/***********************************************************************************************************************
 * VPrimitiveType.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VPRIMITIVETYPE_H_
#define VPRIMITIVETYPE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
}

namespace OOModel {
	class PrimitiveType;
}

namespace OOVisualization {

class OOVISUALIZATION_API VPrimitiveType : public Visualization::ItemWithNode< Visualization::Item, OOModel::PrimitiveType>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VPrimitiveType, Visualization::TextStyle)

	public:
		VPrimitiveType(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VPrimitiveType>("default"));
		virtual ~VPrimitiveType();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Text* vis_;
};

}

#endif /* VPRIMITIVETYPE_H_ */
