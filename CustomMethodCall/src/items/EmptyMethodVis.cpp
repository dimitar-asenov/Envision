/***********************************************************************************************************************
 * EmptyMethodVis.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/EmptyMethodVis.h"

#include "VisualizationBase/headers/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace CustomMethodCall {

ITEM_COMMON_DEFINITIONS(EmptyMethodVis, "item")

EmptyMethodVis::EmptyMethodVis(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, MethodCallExpression>(parent, node, style),
	icon_(new Static(NULL, &style->icon()) ),
	separator_(NULL),
	prefix_(NULL)
{
	layout()->append(icon_);
}

EmptyMethodVis::~EmptyMethodVis()
{
	// These were automatically deleted by LayoutProvider's destructor
	icon_ = NULL;
	separator_ = NULL;
	prefix_ = NULL;
}

void EmptyMethodVis::determineChildren()
{
	layout()->synchronizeFirst(prefix_, node()->prefix());
	layout()->synchronizeMid(separator_, node()->prefix() != NULL, &style()->separator(), 1);

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	icon_->setStyle( &style()->icon());
	if (prefix_) separator_->setStyle( &style()->separator());
}

}
