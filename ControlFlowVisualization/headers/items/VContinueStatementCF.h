/***********************************************************************************************************************
 * VContinueStatementCF.h
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCONTINUESTATEMENTCF_H_
#define VCONTINUESTATEMENTCF_H_

#include "../controlflowvisualization_api.h"
#include "ControlFlowItem.h"

#include "OOVisualization/headers/statements/VContinueStatement.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VContinueStatementCF : public Visualization::ItemWithNode< ControlFlowItem, OOModel::ContinueStatement >
{
		ITEM_COMMON_CUSTOM_STYLENAME(VContinueStatementCF, ControlFlowItemStyle)

	public:
		VContinueStatementCF(Item* parent, NodeType *node, const StyleType *style = Visualization::Styles::item<VContinueStatementCF>("default"));
		virtual ~VContinueStatementCF();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		OOVisualization::VContinueStatement* vis_;
};

}

#endif /* VCONTINUESTATEMENTCF_H_ */
