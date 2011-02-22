/***********************************************************************************************************************
 * VReturnStatement.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VReturnStatement.h"

#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VReturnStatement)

VReturnStatement::VReturnStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<>, ReturnStatement>(parent, node, style),
	symbol_( new Symbol(NULL, &style->symbol() )),
	values_( new VList(NULL, node->values()) )
{
	layout()->append(symbol_);
	layout()->append(values_);
}

VReturnStatement::~VReturnStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	symbol_ = NULL;
	values_ = NULL;
}

void VReturnStatement::determineChildren()
{
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	symbol_->setStyle( &style()->symbol());
	values_->setStyle( &style()->values() );

	layout()->synchronizeLast(values_, node()->values(), &style()->values());
}

}
