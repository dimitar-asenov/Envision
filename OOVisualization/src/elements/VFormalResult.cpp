/***********************************************************************************************************************
 * VFormalResult.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VFormalResult.h"

#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VFormalResult)

VFormalResult::VFormalResult(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<>, FormalResult>(parent, node, style),
	name_(new VText(NULL, node->nameNode(), &style->name()) ),
	type_(NULL)
{
	layout()->append(name_);
}

VFormalResult::~VFormalResult()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = NULL;
	type_ = NULL;
}

void VFormalResult::determineChildren()
{
	layout()->synchronizeFirst(name_, node()->nameNode(), &style()->name());
	layout()->synchronizeLast(type_, node()->type());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );
	name_->setStyle( &style()->name());
}

}
