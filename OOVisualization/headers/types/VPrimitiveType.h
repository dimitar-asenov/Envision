/***********************************************************************************************************************
 * VPrimitiveType.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VPRIMITIVETYPE_H_
#define VPRIMITIVETYPE_H_

#include "../oovisualization_api.h"

#include "OOModel/headers/types/PrimitiveType.h"

#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"

namespace Visualization {
	class Text;
}

namespace OOVisualization {

class OOVISUALIZATION_API VPrimitiveType : public Visualization::ItemWithNode< Visualization::Item, OOModel::PrimitiveType>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VPrimitiveType, Visualization::TextStyle)

	public:
		VPrimitiveType(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
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
