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

ITEM_COMMON_DEFINITIONS(VContinueStatement)

VContinueStatement::VContinueStatement(Item* parent, ContinueStatement* node, const SymbolStyle* style) :
	ModelItem(parent, node, style),
	vis_(new Symbol(this, style))
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
