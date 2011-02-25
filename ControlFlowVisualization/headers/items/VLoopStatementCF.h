/***********************************************************************************************************************
 * VLoopStatementCF.h
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLOOPSTATEMENTCF_H_
#define VLOOPSTATEMENTCF_H_

#include "../controlflowvisualization_api.h"
#include "VLoopStatementCFStyle.h"
#include "VListCF.h"

#include "OOVisualization/headers/statements/VLoopStatement.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VLoopStatementCF : public Visualization::ItemWithNode<ControlFlowItem, OOModel::LoopStatement>
{
	ITEM_COMMON(VLoopStatementCF)

	public:
		VLoopStatementCF(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VLoopStatementCF>("default"));
		virtual ~VLoopStatementCF();

		virtual bool focusChild(FocusTarget location);
		virtual bool sizeDependsOnParent() const;
		virtual bool isEmpty() const;

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* conditionBackground;
		Visualization::SequentialLayout* initStepBackground;
		Visualization::SequentialLayout* updateStepBackground;

		Visualization::Item* condition;
		Visualization::Item* initStep;
		Visualization::Item* updateStep;
		VListCF* body;

		OOVisualization::VLoopStatement* vis_;
};

}

#endif /* VLOOPSTATEMENTCF_H_ */
