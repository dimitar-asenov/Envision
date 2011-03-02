/***********************************************************************************************************************
 * VContinueStatementCF.cpp
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VContinueStatementCF.h"

#include "OOModel/headers/statements/ContinueStatement.h"

using namespace Visualization;
using namespace OOModel;
using namespace OOVisualization;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VContinueStatementCF)

VContinueStatementCF::VContinueStatementCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<ControlFlowItem, ContinueStatement>(parent, node, style),
	vis_(NULL)
{
}

VContinueStatementCF::~VContinueStatementCF()
{
	SAFE_DELETE_ITEM(vis_);
}

void VContinueStatementCF::determineChildren()
{
	synchronizeItem<VContinueStatement>(vis_, node(), NULL);
}

void VContinueStatementCF::updateGeometry(int availableWidth, int availableHeight)
{
	clearConnectors();
	continues_.clear();

	if (! showAsControlFlow() ) Item::updateGeometry(vis_, availableWidth, availableHeight);
	else
	{
		vis_->setPos(style()->pinLength(), style()->pinLength());
		setSize(vis_->width() + 2*style()->pinLength(), vis_->height() + 2*style()->pinLength());
	}

	if (preferredContinueExit_ == ControlFlowItem::EXIT_LEFT)
	{
		continues_.append( QPoint(0, height()/2));
		addConnector(continues_.first() + QPoint(style()->pinLength(),0), continues_.first(), false );
	}
	else
	{
		continues_.append( QPoint(width(), height()/2));
		addConnector(continues_.first() - QPoint(style()->pinLength(),0), continues_.first(), false );
	}

	entrance_ = QPoint(width()/2, 0);
	exit_ = QPoint(0,0);
	addConnector(entrance_, entrance_ + QPoint(0, style()->pinLength()), true);
}

bool VContinueStatementCF::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
