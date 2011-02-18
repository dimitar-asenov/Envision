/***********************************************************************************************************************
 * VMethodCallExpression.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VMethodCallExpression.h"

#include "OOModel/headers/expressions/MethodCallExpression.h"

#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VMethodCallExpression)

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

	bool newHasPrefix = node()->prefix() != NULL;

	if (newHasPrefix)
	{
		if (prefix_ && prefix_->node() != node()->prefix())
		{
			layout()->remove(0);
			layout()->remove(0);
		}

		if (!prefix_)
		{
			prefix_ = renderer()->render(NULL,node()->prefix());
			separator_ = new Symbol(NULL, &style()->separator());
			layout()->prepend(separator_);
			layout()->prepend(prefix_);
		}
	}
	else
	{
		if (prefix_)
		{
			layout()->remove(0);
			layout()->remove(0);
		}
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->container());
	name_->setStyle( &style()->name());
	arguments_->setStyle( &style()->arguments() );
	if (prefix_)
	{
		separator_->setStyle( &style()->separator());
	}

	name_->setText(node()->ref()->path().split(',').last().split(':').last());
}

}
