/***********************************************************************************************************************
 * VNewExpression.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VNEWEXPRESSION_H_
#define VNEWEXPRESSION_H_

#include "../oovisualization_api.h"
#include "VNewExpressionStyle.h"

#include "OOModel/headers/expressions/NewExpression.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Static;
}

namespace OOVisualization {

class OOVISUALIZATION_API VNewExpression : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::NewExpression>
{
	ITEM_COMMON(VNewExpression)

	public:
		VNewExpression(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VNewExpression();

	protected:
		void determineChildren();

	private:
		Visualization::Static* newSymbol_;
		Visualization::Item* type_;
		Visualization::Item* amount_;
};

}

#endif /* VNEWEXPRESSION_H_ */
