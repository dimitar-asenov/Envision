/***********************************************************************************************************************
 * VArrayInitializer.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VArrayInitializer.h"

#include "VisualizationBase/headers/layouts/GridLayout.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VArrayInitializer)

VArrayInitializer::VArrayInitializer(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<GridLayout>, ArrayInitializer>(parent, node, style),
	values_( new VList(NULL, node->values()) )
{
	layout()->setGridSize(1,1, false);
	layout()->set(values_,0,0, true);
}

VArrayInitializer::~VArrayInitializer()
{
	// These were automatically deleted by LayoutProvider's destructor
	values_ = NULL;
}

void VArrayInitializer::determineChildren()
{
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	values_->setStyle( &style()->values() );

	layout()->synchronize(values_, node()->values(), &style()->values(), 0,0);
}

}
