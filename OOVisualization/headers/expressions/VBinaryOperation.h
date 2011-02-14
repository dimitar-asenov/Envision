/***********************************************************************************************************************
 * VBinaryOperation.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBINARYOPERATION_H_
#define VBINARYOPERATION_H_

#include "../oovisualization_api.h"
#include "OperatorStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class BinaryOperation;
}

namespace OOVisualization {

class OOVISUALIZATION_API VBinaryOperation : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBinaryOperation, Visualization::ModelItem, OperatorSequenceStyle)

	public:
		VBinaryOperation(Item* parent, OOModel::BinaryOperation* node, const OperatorSequenceStyle* style = Visualization::Styles::item<VBinaryOperation>("default"));
		virtual ~VBinaryOperation();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Symbol* pre_;
		Visualization::Symbol* in_;
		Visualization::Symbol* post_;
		Visualization::ModelItem* left_;
		Visualization::ModelItem* right_;
};

}

#endif /* VBINARYOPERATION_H_ */
