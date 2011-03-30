/***********************************************************************************************************************
 * VThisExpression.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VTHISEXPRESSION_H_
#define VTHISEXPRESSION_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/Static.h"

#include "OOModel/headers/expressions/ThisExpression.h"

namespace OOVisualization {

class OOVISUALIZATION_API VThisExpression : public Visualization::ItemWithNode< Visualization::Item, OOModel::ThisExpression>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VThisExpression, Visualization::StaticStyle)

	public:
		VThisExpression(Item* parent, NodeType *expr, const StyleType *style = itemStyles().get());
		virtual ~VThisExpression();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Static* vis_;
};

}

#endif /* VTHISEXPRESSION_H_ */
