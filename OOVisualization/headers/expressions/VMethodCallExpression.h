/***********************************************************************************************************************
 * VMethodCallExpression.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODCALLEXPRESSION_H_
#define VMETHODCALLEXPRESSION_H_

#include "../oovisualization_api.h"
#include "VMethodCallExpressionStyle.h"

#include "OOModel/headers/expressions/MethodCallExpression.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Text;
	class Symbol;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VMethodCallExpression : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallExpression>
{
	ITEM_COMMON(VMethodCallExpression)

	public:
		VMethodCallExpression(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VMethodCallExpression();

	protected:
		void determineChildren();

	private:
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::Item* prefix_;
		Visualization::VList* arguments_;
};

}

#endif /* VMETHODCALLEXPRESSION_H_ */
