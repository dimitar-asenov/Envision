/***********************************************************************************************************************
 * VThisExpression.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VTHISEXPRESSION_H_
#define VTHISEXPRESSION_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/ThisExpression.h"

namespace OOVisualization {

class OOVISUALIZATION_API VThisExpression : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VThisExpression, Visualization::ModelItem, Visualization::SymbolStyle)

	public:
		VThisExpression(Item* parent, OOModel::ThisExpression *expr, const Visualization::SymbolStyle *style = Visualization::Styles::item<VThisExpression>("default"));
		virtual ~VThisExpression();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Symbol* vis_;
};

}

#endif /* VTHISEXPRESSION_H_ */
