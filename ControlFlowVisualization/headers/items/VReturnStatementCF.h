/***********************************************************************************************************************
 * VReturnStatementCF.h
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VRETURNSTATEMENTCF_H_
#define VRETURNSTATEMENTCF_H_

#include "../controlflowvisualization_api.h"
#include "ControlFlowItem.h"

#include "OOVisualization/headers/statements/VReturnStatement.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VReturnStatementCF : public Visualization::ItemWithNode< ControlFlowItem, OOModel::ReturnStatement >
{
		ITEM_COMMON_CUSTOM_STYLENAME(VReturnStatementCF, ControlFlowItemStyle)

	public:
		VReturnStatementCF(Item* parent, NodeType *node, const StyleType *style = Visualization::Styles::item<VReturnStatementCF>("default"));
		virtual ~VReturnStatementCF();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		OOVisualization::VReturnStatement* vis_;
};

}

#endif /* VRETURNSTATEMENTCF_H_ */
