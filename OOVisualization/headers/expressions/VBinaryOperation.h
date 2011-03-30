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

namespace Visualization {
	class Static;
}

namespace OOVisualization {

class OOVISUALIZATION_API VBinaryOperation : public Visualization::ItemWithNode<Visualization::LayoutProvider<>, OOModel::BinaryOperation>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBinaryOperation, OperatorSequenceStyle)

	public:
		VBinaryOperation(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VBinaryOperation();

	protected:
		void determineChildren();

	private:
		Visualization::Static* pre_;
		Visualization::Static* in_;
		Visualization::Static* post_;
		Visualization::Item* left_;
		Visualization::Item* right_;
};

}

#endif /* VBINARYOPERATION_H_ */
