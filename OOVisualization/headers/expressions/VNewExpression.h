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
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class NewExpression;
}

namespace OOVisualization {

class OOVISUALIZATION_API VNewExpression : public Visualization::ModelItem
{
	ITEM_COMMON(VNewExpression)

	public:
		VNewExpression(Item* parent, OOModel::NewExpression* node, const VNewExpressionStyle* style = Visualization::Styles::item<VNewExpression>("default"));
		virtual ~VNewExpression();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Symbol* newSymbol_;
		Visualization::ModelItem* type_;
		Visualization::Symbol* amountSymbol_;
		Visualization::ModelItem* amount_;
};

}

#endif /* VNEWEXPRESSION_H_ */
