/***********************************************************************************************************************
 * VReferenceExpression.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VREFERENCEEXPRESSION_H_
#define VREFERENCEEXPRESSION_H_

#include "../oovisualization_api.h"
#include "VReferenceExpressionStyle.h"

#include "OOModel/headers/expressions/ReferenceExpression.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Text;
	class Static;
}

namespace OOVisualization {

class OOVISUALIZATION_API VReferenceExpression : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::ReferenceExpression>
{
	ITEM_COMMON(VReferenceExpression)

	public:
		VReferenceExpression(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VReferenceExpression();

	protected:
		void determineChildren();

	private:
		Visualization::Text* name_;
		Visualization::Static* separator_;
		Visualization::Item* prefix_;
};

}

#endif /* VREFERENCEEXPRESSION_H_ */
