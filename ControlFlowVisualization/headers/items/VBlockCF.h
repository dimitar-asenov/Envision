/***********************************************************************************************************************
 * VBlockCF.h
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBLOCKCF_H_
#define VBLOCKCF_H_

#include "../controlflowvisualization_api.h"
#include "VBlockCFStyle.h"
#include "VListCF.h"

#include "OOVisualization/headers/statements/VBlock.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VBlockCF : public Visualization::ItemWithNode<ControlFlowItem, OOModel::Block>
{
	ITEM_COMMON(VBlockCF)

	public:
		VBlockCF(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VBlockCF>("default"));
		virtual ~VBlockCF();

		virtual bool focusChild(FocusTarget location);
		virtual bool sizeDependsOnParent() const;
		virtual bool isEmpty() const;

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		VListCF* statements;
		OOVisualization::VBlock* vis_;
};

}

#endif /* VBLOCKCF_H_ */
