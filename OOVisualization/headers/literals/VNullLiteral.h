/***********************************************************************************************************************
 * VNullLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VNULLLITERAL_H_
#define VNULLLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/Static.h"

#include "OOModel/headers/expressions/NullLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VNullLiteral : public Visualization::ItemWithNode< Visualization::Item, OOModel::NullLiteral>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VNullLiteral, Visualization::StaticStyle)

	public:
		VNullLiteral(Item* parent, NodeType* literal, const StyleType* style = itemStyles().get());
		virtual ~VNullLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Static* vis_;
};

}

#endif /* VNULLLITERAL_H_ */
