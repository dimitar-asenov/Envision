/***********************************************************************************************************************
 * VIfStatementCF.h
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VIFSTATEMENTCF_H_
#define VIFSTATEMENTCF_H_

#include "../controlflowvisualization_api.h"
#include "VIfStatementCFStyle.h"
#include "VListCF.h"

#include "OOVisualization/headers/statements/VIfStatement.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VIfStatementCF : public Visualization::ItemWithNode<ControlFlowItem, OOModel::IfStatement>
{
	ITEM_COMMON(VIfStatementCF)

	public:
		VIfStatementCF(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VIfStatementCF();

		virtual bool focusChild(FocusTarget location);
		virtual bool sizeDependsOnParent() const;
		virtual bool isEmpty() const;

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* conditionBackground;

		Visualization::Item* condition;
		VListCF* thenBranch;
		VListCF* elseBranch;

		OOVisualization::VIfStatement* vis_;
};

}

#endif /* VIFSTATEMENTCF_H_ */
