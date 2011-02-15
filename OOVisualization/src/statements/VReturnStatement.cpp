/***********************************************************************************************************************
 * VReturnStatement.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VReturnStatement.h"

#include "OOModel/headers/statements/ReturnStatement.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VReturnStatement)

VReturnStatement::VReturnStatement(Item* parent, ReturnStatement* node, const VReturnStatementStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	symbol_( new Symbol(NULL, &style->symbol() )),
	values_( new VList(NULL, node->values()) )
{
	container_->append(symbol_);
	container_->append(values_);
}

VReturnStatement::~VReturnStatement()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	symbol_ = NULL;
	values_ = NULL;
}

void VReturnStatement::determineChildren()
{
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->container());
	symbol_->setStyle( &style()->symbol());
	values_->setStyle( &style()->values() );
}

void VReturnStatement::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VReturnStatement::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
