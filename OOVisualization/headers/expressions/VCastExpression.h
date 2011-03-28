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

#include "OOModel/headers/expressions/CastExpression.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Symbol;
}

namespace OOVisualization {

class OOVISUALIZATION_API VCastExpression : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::CastExpression>
{
	ITEM_COMMON(VCastExpression)

	public:
		VCastExpression(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VCastExpression();

	protected:
		void determineChildren();

	private:
		Visualization::Item* type_;
		Visualization::Item* expr_;
};

}

#endif /* VCASTEXPRESSION_H_ */
