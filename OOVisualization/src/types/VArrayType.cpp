/***********************************************************************************************************************
 * VArrayType.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "types/VArrayType.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VArrayType, "item")

VArrayType::VArrayType(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, ArrayType>(parent, node, style),
	symbol_( new Static(NULL, &style->symbol())),
	type_(NULL)
{
	layout()->append(symbol_);
}

VArrayType::~VArrayType()
{
	// These were automatically deleted by LayoutProvider's destructor
	symbol_ = NULL;
	type_ = NULL;
}

void VArrayType::determineChildren()
{
	layout()->synchronizeFirst(type_, node()->type());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	symbol_->setStyle( &style()->symbol());
}

}
