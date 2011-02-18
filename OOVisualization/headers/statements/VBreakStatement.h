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
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/statements/BreakStatement.h"

namespace OOVisualization {

class OOVISUALIZATION_API VBreakStatement : public Visualization::ItemWithNode< Visualization::Item, OOModel::BreakStatement>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBreakStatement, Visualization::SymbolStyle)

	public:
		VBreakStatement(Item* parent, NodeType *node, const StyleType *style = Visualization::Styles::item<VBreakStatement>("default"));
		virtual ~VBreakStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Symbol* vis_;
};

}

#endif /* VBREAKSTATEMENT_H_ */
