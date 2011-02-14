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

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class ReferenceExpression;
}

namespace OOVisualization {

class OOVISUALIZATION_API VReferenceExpression : public Visualization::ModelItem
{
	ITEM_COMMON(VReferenceExpression, Visualization::ModelItem)

	public:
		VReferenceExpression(Item* parent, OOModel::ReferenceExpression* node, const VReferenceExpressionStyle* style = Visualization::Styles::item<VReferenceExpression>("default"));
		virtual ~VReferenceExpression();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::ModelItem* prefix_;
};

}

#endif /* VREFERENCEEXPRESSION_H_ */
