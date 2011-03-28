/***********************************************************************************************************************
 * VMethodCallExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VMethodCallExpression.h"

#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VMethodCallExpression, "item")

VMethodCallExpression::VMethodCallExpression(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, MethodCallExpression>(parent, node, style),
	name_(new Text(NULL, &style->name()) ),
	separator_(NULL),
	prefix_(NULL),
	arguments_(new VList(NULL, node->arguments(), &style->arguments()))
{
	layout()->append(name_);
	layout()->append(arguments_);
}

VMethodCallExpression::~VMethodCallExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = NULL;
	separator_ = NULL;
	prefix_ = NULL;
	arguments_ = NULL;
}

void VMethodCallExpression::determineChildren()
{
	layout()->synchronizeFirst(prefix_, node()->prefix());
	layout()->synchronizeMid(separator_, node()->prefix() != NULL, &style()->separator(), 1);
	layout()->synchronizeLast(arguments_, node()->arguments(), &style()->arguments());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	name_->setStyle( &style()->name());
	arguments_->setStyle( &style()->arguments() );
	if (prefix_) separator_->setStyle( &style()->separator());

	name_->setText(node()->ref()->path().split(',').last().split(':').last());
}

}
