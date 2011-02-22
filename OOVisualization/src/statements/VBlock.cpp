/***********************************************************************************************************************
 * VBlock.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VBlock.h"

#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VBlock)

VBlock::VBlock(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<Item, Block>(parent, node, style),
	items_( new VList(this, node->items()) )
{
}

VBlock::~VBlock()
{
	SAFE_DELETE_ITEM(items_);
}

void VBlock::determineChildren()
{
	synchronizeItem(items_, node()->items(), &style()->items());
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	items_->setStyle( &style()->items() );
}

void VBlock::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(items_, availableWidth, availableHeight );
}

bool VBlock::focusChild(FocusTarget location)
{
	return items_->focusChild(location);
}

}
