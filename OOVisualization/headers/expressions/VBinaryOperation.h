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

#include "OOModel/headers/expressions/BinaryOperation.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
}

namespace OOVisualization {

class OOVISUALIZATION_API VBinaryOperation : public Visualization::ItemWithNode<Visualization::LayoutProvider<>, OOModel::BinaryOperation>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBinaryOperation, OperatorSequenceStyle)

	public:
		VBinaryOperation(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VBinaryOperation>("default"));
		virtual ~VBinaryOperation();

	protected:
		void determineChildren();

	private:
		Visualization::Symbol* pre_;
		Visualization::Symbol* in_;
		Visualization::Symbol* post_;
		Visualization::Item* left_;
		Visualization::Item* right_;
};

}

#endif /* VBINARYOPERATION_H_ */
