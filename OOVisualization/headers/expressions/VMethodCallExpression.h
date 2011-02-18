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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class Symbol;
	class VList;
}

namespace OOModel {
	class MethodCallExpression;
}

namespace OOVisualization {

class OOVISUALIZATION_API VMethodCallExpression : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallExpression>
{
	ITEM_COMMON(VMethodCallExpression)

	public:
		VMethodCallExpression(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VMethodCallExpression>("default"));
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
