/***********************************************************************************************************************
 * VBreakStatementCF.cpp
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBreakStatementCF.h"

#include "OOModel/headers/statements/BreakStatement.h"

using namespace Visualization;
using namespace OOModel;
using namespace OOVisualization;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VBreakStatementCF)

VBreakStatementCF::VBreakStatementCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<ControlFlowItem, BreakStatement>(parent, node, style),
	vis_(NULL)
{
}

VBreakStatementCF::~VBreakStatementCF()
{
	SAFE_DELETE_ITEM(vis_);
}

void VBreakStatementCF::determineChildren()
{
	synchronizeItem<VBreakStatement>(vis_, node(), NULL);
}

void VBreakStatementCF::updateGeometry(int availableWidth, int availableHeight)
{
	clearConnectors();
	breaks_.clear();

	if (! showAsControlFlow() )
	{
		Item::updateGeometry(vis_, availableWidth, availableHeight);
		return;
	}
	else
	{
		vis_->setPos(style()->pinLength(), style()->pinLength());
		setSize(vis_->width() + 2*style()->pinLength(), vis_->height() + 2*style()->pinLength());
	}

	if (preferredBreakExit_ == ControlFlowItem::EXIT_LEFT)
	{
		breaks_.append( QPoint(0, height()/2));
		addConnector(breaks_.first() + QPoint(style()->pinLength(),0), breaks_.first(), false);
	}
	else
	{
		breaks_.append( QPoint(width(), height()/2));
		addConnector(breaks_.first()- QPoint(style()->pinLength(),0), breaks_.first(), false);
	}

	entrance_ = QPoint(width()/2, 0);
	exit_ = QPoint(0,0);
	addConnector(entrance_, entrance_ + QPoint(0, style()->pinLength()), true);
}

bool VBreakStatementCF::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
