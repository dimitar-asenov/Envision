/***********************************************************************************************************************
 * VBreakStatement.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBREAKSTATEMENT_H_
#define VBREAKSTATEMENT_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/Static.h"

#include "OOModel/headers/statements/BreakStatement.h"

namespace OOVisualization {

class OOVISUALIZATION_API VBreakStatement : public Visualization::ItemWithNode< Visualization::Item, OOModel::BreakStatement>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBreakStatement, Visualization::StaticStyle)

	public:
		VBreakStatement(Item* parent, NodeType *node, const StyleType *style = itemStyles().get());
		virtual ~VBreakStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Static* vis_;
};

}

#endif /* VBREAKSTATEMENT_H_ */
