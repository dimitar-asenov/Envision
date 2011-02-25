/***********************************************************************************************************************
 * ControlFlowItem.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/ControlFlowItem.h"
#include "items/VMethodCF.h"

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(ControlFlowItem)

ControlFlowItem::ControlFlowItem(Item* parent,  const StyleType* style) :
	Item(parent, style)
{
}

bool ControlFlowItem::showAsControlFlow() const
{
	QGraphicsItem* item = parentItem();
	while (item)
	{
		VMethodCF* met = dynamic_cast<VMethodCF*> (item);
		if (met) return met->style()->showAsControlFlow();

		item = item->parentItem();
	}

	return false;
}

}
