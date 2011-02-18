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

class OOVISUALIZATION_API VNamedType : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VNamedType, VReferenceExpressionStyle)

	public:
		VNamedType(Item* parent, OOModel::NamedType* node, const VReferenceExpressionStyle* style = Visualization::Styles::item<VReferenceExpression>("default"));
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
