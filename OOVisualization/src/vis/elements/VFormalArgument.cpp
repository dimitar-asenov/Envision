/***********************************************************************************************************************
 * VFormalArgument.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/elements/VFormalArgument.h"

#include "OOModel/headers/FormalArgument.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VFormalArgument)

VFormalArgument::VFormalArgument(Item* parent, FormalArgument* node, const VFormalArgumentStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	name_(new VText(NULL, node->nameNode(), &style->name()) ),
	type_(NULL)
{
	container_->append(name_);
}

VFormalArgument::~VFormalArgument()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	name_ = NULL;
	type_ = NULL;
}

void VFormalArgument::determineChildren()
{
	if (type_ == NULL)
	{
		FormalArgument* node = static_cast<FormalArgument*> (getNode());
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

void VFormalArgument::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VFormalArgument::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
