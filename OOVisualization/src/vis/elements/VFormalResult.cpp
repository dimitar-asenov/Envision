/***********************************************************************************************************************
 * VFormalResult.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/elements/VFormalResult.h"

#include "OOModel/headers/FormalResult.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VFormalResult)

VFormalResult::VFormalResult(Item* parent, FormalResult* node, const VFormalResultStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	name_(new VText(NULL, node->nameNode(), &style->name()) ),
	type_(NULL)
{
	container_->append(name_);
}

VFormalResult::~VFormalResult()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	name_ = NULL;
	type_ = NULL;
}

void VFormalResult::determineChildren()
{
	if (type_ == NULL)
	{
		FormalResult* node = static_cast<FormalResult*> (getNode());
		type_ = renderer()->render(NULL, node->type());
		container_->append( type_ );
	}
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->container() );
	name_->setStyle( &style()->name());
}

void VFormalResult::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VFormalResult::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
