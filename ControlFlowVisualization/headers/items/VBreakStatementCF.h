/***********************************************************************************************************************
 * VBreakStatementCF.h
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBREAKSTATEMENTCF_H_
#define VBREAKSTATEMENTCF_H_

#include "../controlflowvisualization_api.h"
#include "ControlFlowItem.h"

#include "OOVisualization/headers/statements/VBreakStatement.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VBreakStatementCF : public Visualization::ItemWithNode< ControlFlowItem, OOModel::BreakStatement >
{
		ITEM_COMMON_CUSTOM_STYLENAME(VBreakStatementCF, ControlFlowItemStyle)

	public:
		VBreakStatementCF(Item* parent, NodeType *node, const StyleType *style = itemStyles().get());
		virtual ~VBreakStatementCF();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		OOVisualization::VBreakStatement* vis_;
};

}

#endif /* VBREAKSTATEMENTCF_H_ */
