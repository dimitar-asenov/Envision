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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
}

namespace OOModel {
	class NewExpression;
}

namespace OOVisualization {

class OOVISUALIZATION_API VNewExpression : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::NewExpression>
{
	ITEM_COMMON(VNewExpression)

	public:
		VNewExpression(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VNewExpression>("default"));
		virtual ~VNewExpression();

	protected:
		void determineChildren();

	private:
		Visualization::Symbol* newSymbol_;
		Visualization::Item* type_;
		Visualization::Symbol* amountSymbol_;
		Visualization::Item* amount_;
};

}

#endif /* VNEWEXPRESSION_H_ */
