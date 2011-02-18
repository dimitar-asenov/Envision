/***********************************************************************************************************************
 * VUnaryOperation.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VUNARYOPERATION_H_
#define VUNARYOPERATION_H_

#include "../oovisualization_api.h"
#include "OperatorStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class UnaryOperation;
}

namespace OOVisualization {

class OOVISUALIZATION_API VUnaryOperation : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VUnaryOperation, OperatorSequenceStyle)

	public:
		VUnaryOperation(Item* parent, OOModel::UnaryOperation* node, const OperatorSequenceStyle* style = Visualization::Styles::item<VUnaryOperation>("default"));
		virtual ~VUnaryOperation();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Symbol* pre_;
		Visualization::Symbol* post_;
		Visualization::ModelItem* expr_;
};

}

#endif /* VUNARYOPERATION_H_ */
