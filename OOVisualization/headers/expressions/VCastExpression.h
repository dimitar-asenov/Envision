/***********************************************************************************************************************
 * VCastExpression.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCASTEXPRESSION_H_
#define VCASTEXPRESSION_H_

#include "../oovisualization_api.h"
#include "VCastExpressionStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class CastExpression;
}

namespace OOVisualization {

class OOVISUALIZATION_API VCastExpression : public Visualization::ModelItem
{
	ITEM_COMMON(VCastExpression)

	public:
		VCastExpression(Item* parent, OOModel::CastExpression* node, const VCastExpressionStyle* style = Visualization::Styles::item<VCastExpression>("default"));
		virtual ~VCastExpression();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Symbol* separator_;
		Visualization::ModelItem* type_;
		Visualization::ModelItem* expr_;
};

}

#endif /* VCASTEXPRESSION_H_ */
