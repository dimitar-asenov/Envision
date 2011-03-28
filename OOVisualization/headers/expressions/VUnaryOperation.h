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

#include "OOModel/headers/expressions/UnaryOperation.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Symbol;
}

namespace OOVisualization {

class OOVISUALIZATION_API VUnaryOperation : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::UnaryOperation>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VUnaryOperation, OperatorSequenceStyle)

	public:
		VUnaryOperation(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VUnaryOperation();

	protected:
		virtual void determineChildren();

	private:
		Visualization::Symbol* pre_;
		Visualization::Symbol* post_;
		Visualization::Item* expr_;
};

}

#endif /* VUNARYOPERATION_H_ */
