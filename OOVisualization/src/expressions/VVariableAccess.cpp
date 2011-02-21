/***********************************************************************************************************************
 * VVariableAccess.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VVariableAccess.h"

#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VVariableAccess)

VVariableAccess::VVariableAccess(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, VariableAccess>(parent, node, style),
	name_(new Text(NULL, &style->name()) ),
	separator_(NULL),
	prefix_(NULL)
{
	layout()->append(name_);
}

VVariableAccess::~VVariableAccess()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = NULL;
	separator_ = NULL;
	prefix_ = NULL;
}

void VVariableAccess::determineChildren()
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
	layout()->setStyle( &style()->layout());
	name_->setStyle( &style()->name());
	if (prefix_)
	{
		separator_->setStyle( &style()->separator());
	}

	name_->setText(node()->ref()->path().split(',').last().split(':').last());
}

}
