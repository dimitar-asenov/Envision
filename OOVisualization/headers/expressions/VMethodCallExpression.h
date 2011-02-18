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
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class Symbol;
	class SequentialLayout;
	class VList;
}

namespace OOModel {
	class MethodCallExpression;
}

namespace OOVisualization {

class OOVISUALIZATION_API VMethodCallExpression : public Visualization::ModelItem
{
	ITEM_COMMON(VMethodCallExpression)

	public:
		VMethodCallExpression(Item* parent, OOModel::MethodCallExpression* node, const VMethodCallExpressionStyle* style = Visualization::Styles::item<VMethodCallExpression>("default"));
		virtual ~VMethodCallExpression();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::ModelItem* prefix_;
		Visualization::VList* arguments_;
};

}

#endif /* VMETHODCALLEXPRESSION_H_ */
