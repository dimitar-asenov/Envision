/***********************************************************************************************************************
 * VNamedType.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VNAMEDTYPE_H_
#define VNAMEDTYPE_H_

#include "../expressions/VReferenceExpression.h"

#include "OOModel/headers/types/NamedType.h"

namespace OOVisualization {

class OOVISUALIZATION_API VNamedType : public Visualization::ItemWithNode< Visualization::Item, OOModel::NamedType>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VNamedType, VReferenceExpressionStyle)

	public:
		VNamedType(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VNamedType();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		VReferenceExpression* vis_;
};

}

#endif /* VNAMEDTYPE_H_ */
