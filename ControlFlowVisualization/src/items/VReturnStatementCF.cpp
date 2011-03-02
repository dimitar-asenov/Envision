/***********************************************************************************************************************
 * VReturnStatementCF.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VReturnStatementCF.h"

#include "OOModel/headers/statements/ReturnStatement.h"

using namespace Visualization;
using namespace OOModel;
using namespace OOVisualization;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VReturnStatementCF)

VReturnStatementCF::VReturnStatementCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<ControlFlowItem, ReturnStatement>(parent, node, style),
	vis_(NULL)
{
}

VReturnStatementCF::~VReturnStatementCF()
{
	SAFE_DELETE_ITEM(vis_);
}

void VReturnStatementCF::determineChildren()
{
	synchronizeItem<VReturnStatement>(vis_, node(), NULL);
}

void VReturnStatementCF::updateGeometry(int availableWidth, int availableHeight)
{
	clearConnectors();

	if (! showAsControlFlow() ) Item::updateGeometry(vis_, availableWidth, availableHeight);
	else
	{
		vis_->setPos(style()->pinLength(), style()->pinLength());
		setSize(vis_->width() + 2*style()->pinLength(), vis_->height() + 2*style()->pinLength());
	}

	entrance_ = QPoint(width()/2, 0);
	addConnector(entrance_, entrance_ + QPoint(0, style()->pinLength()), true);
}

bool VReturnStatementCF::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
