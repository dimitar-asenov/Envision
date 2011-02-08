/***********************************************************************************************************************
 * VReferenceExpression.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VReferenceExpression.h"

#include "OOModel/headers/expressions/ReferenceExpression.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Text.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VReferenceExpression)

VReferenceExpression::VReferenceExpression(Item* parent, ReferenceExpression* node, const VReferenceExpressionStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	name_(new Text(NULL, &style->name()) ),
	separator_(NULL),
	prefix_(NULL)
{
	container_->append(name_);
}

VReferenceExpression::~VReferenceExpression()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	name_ = NULL;
	separator_ = NULL;
	prefix_ = NULL;
}

void VReferenceExpression::determineChildren()
{
	ReferenceExpression* node = static_cast<ReferenceExpression*> (getNode());

	bool newHasPrefix = node->prefix() != NULL;

	if (newHasPrefix)
	{
		if (prefix_ && prefix_->getNode() != node->prefix())
		{
			container_->remove(0);
			container_->remove(0);
		}

		if (!prefix_)
		{
			prefix_ = renderer()->render(NULL,node->prefix());
			separator_ = new Text(NULL);
			container_->prepend(separator_);
			container_->prepend(prefix_);
			separator_->setText(".");
		}
	}
	else
	{
		if (prefix_)
		{
			container_->remove(0);
			container_->remove(0);
		}
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->container());
	name_->setStyle( &style()->name());
	if (prefix_)
	{
		separator_->setStyle( &style()->separator());
	}

	name_->setText(node->ref()->path().split(',').last().split(':').last());
}

void VReferenceExpression::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VReferenceExpression::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
