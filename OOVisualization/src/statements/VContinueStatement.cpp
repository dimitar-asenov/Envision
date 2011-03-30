/***********************************************************************************************************************
 * VContinueStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VContinueStatement.h"

#include "OOModel/headers/statements/ContinueStatement.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VContinueStatement, "item")

VContinueStatement::VContinueStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<Item, ContinueStatement>(parent, node, style),
	vis_(new Static(this, style))
{
}

VContinueStatement::~VContinueStatement()
{
	SAFE_DELETE_ITEM(vis_);
}

void VContinueStatement::determineChildren()
{
	vis_->setStyle( style());
}

void VContinueStatement::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VContinueStatement::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
