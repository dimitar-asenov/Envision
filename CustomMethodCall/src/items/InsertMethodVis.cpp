/***********************************************************************************************************************
 * InsertMethodVis.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/InsertMethodVis.h"

#include "VisualizationBase/headers/icons/SVGIcon.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace CustomMethodCall {

ITEM_COMMON_DEFINITIONS(InsertMethodVis, "item")

InsertMethodVis::InsertMethodVis(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, MethodCallStatement>(parent, node, style),
	icon_(new SVGIcon(NULL, &style->icon()) ),
	separator_(NULL),
	prefix_(NULL),
	arguments_(new VList(NULL, node->arguments(), &style->arguments()))
{
	layout()->append(icon_);
	layout()->append(arguments_);
}

InsertMethodVis::~InsertMethodVis()
{
	// These were automatically deleted by LayoutProvider's destructor
	icon_ = NULL;
	separator_ = NULL;
	prefix_ = NULL;
	arguments_ = NULL;
}

void InsertMethodVis::determineChildren()
{
	layout()->synchronizeFirst(prefix_, node()->prefix());
	layout()->synchronizeMid(separator_, node()->prefix() != NULL, &style()->separator(), 1);
	layout()->synchronizeLast(arguments_, node()->arguments(), &style()->arguments());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	icon_->setStyle( &style()->icon());
	arguments_->setStyle( &style()->arguments() );
	if (prefix_) separator_->setStyle( &style()->separator());
}

}
