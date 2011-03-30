/***********************************************************************************************************************
 * VStringLiteral.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VStringLiteral.h"

#include "VisualizationBase/headers/items/Static.h"
#include "ModelBase/headers/Model.h"

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStringLiteral, "item")

VStringLiteral::VStringLiteral(Item* parent, NodeType* literal, const StyleType* style) :
	ItemWithNode< LayoutProvider<>, StringLiteral>(parent, literal, style),
	pre_( NULL ),
	post_( NULL ),
	vis_(NULL)
{
}

VStringLiteral::~VStringLiteral()
{
	// These were automatically deleted by LayoutProvider's destructor
	pre_ = NULL;
	post_ = NULL;
	vis_ = NULL;
}

void VStringLiteral::determineChildren()
{
	int index = 0;
	layout()->synchronizeFirst(pre_ , !style()->preSymbol().isEmpty(), &style()->preSymbol());
	index += pre_?1:0;

	layout()->synchronizeMid(vis_, node()->valueNode(), &style()->string(), index);
	index += vis_?1:0;

	layout()->synchronizeLast(post_ , !style()->postSymbol().isEmpty(), &style()->postSymbol());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	vis_->setStyle( &style()->string() );
	if (pre_) pre_->setStyle( &style()->preSymbol());
	if (post_) post_->setStyle( &style()->postSymbol());
}

}
