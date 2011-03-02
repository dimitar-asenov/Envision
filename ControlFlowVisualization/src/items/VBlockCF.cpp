/***********************************************************************************************************************
 * VBlockCF.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBlockCF.h"

using namespace OOVisualization;
using namespace Visualization;
using namespace OOModel;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VBlockCF)

VBlockCF::VBlockCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<ControlFlowItem, Block>(parent, node, style),
	statements( NULL ),
	vis_(NULL)
{
}

VBlockCF::~VBlockCF()
{
	SAFE_DELETE_ITEM(statements);
	SAFE_DELETE_ITEM(vis_);
}

bool VBlockCF::sizeDependsOnParent() const
{
	if (showAsControlFlow()) return statements->sizeDependsOnParent();
	else return vis_->sizeDependsOnParent();
}

bool VBlockCF::isEmpty() const
{
	if (showAsControlFlow()) return statements->isEmpty();
	else return vis_->isEmpty();
}

bool VBlockCF::focusChild(FocusTarget location)
{
	if (showAsControlFlow()) return statements->focusChild(location);
	else return vis_->focusChild(location);
}

void VBlockCF::determineChildren()
{
	if (showAsControlFlow())
	{
		SAFE_DELETE_ITEM(vis_);

		synchronizeItem(statements, node()->items(), &style()->statements());

		statements->setStyle(&style()->statements());
		statements->setPreferredBreakExit(ControlFlowItem::EXIT_LEFT);
		statements->setPreferredContinueExit(ControlFlowItem::EXIT_RIGHT);
	}
	else
	{
		SAFE_DELETE_ITEM(statements);

		synchronizeItem<VBlock>(vis_, node(), NULL);
	}
}

void VBlockCF::updateGeometry(int availableWidth, int availableHeight)
{
	clearConnectors();

	if (!showAsControlFlow())
	{
		Item::updateGeometry(vis_, availableWidth, availableHeight);
		return;
	}

	// TODO this, and add breaks and continues
}

}
