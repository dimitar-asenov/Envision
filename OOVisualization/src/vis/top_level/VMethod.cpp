/***********************************************************************************************************************
 * VMethod.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VMethod.h"
#include "OOVisualizationException.h"
#include "vis/VVisibility.h"

#include "OOModel/headers/Method.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VMethod)

VMethod::VMethod(Item* parent, Method* node, const VMethodStyle* style) :
	ModelItem(parent, node, style),
	layout_( new PanelBorderLayout(this, &style->border()) ),
	name_(new VText(NULL, node->nameNode(), &style->name()) ),
	visibility_(new VVisibility(NULL, node->visibilityNode(), &style->visibility()) ),
	header_( new SequentialLayout(NULL, &style->header()) ),
	nameContainer_( new SequentialLayout(NULL, &style->nameContainer()) ),
	content_( new SequentialLayout(NULL, &style->content()) ),
	arguments_(new SequentialLayout(NULL, &style->arguments()) ),
	results_(new SequentialLayout(NULL, &style->results()) )
{
	layout_->setTop(true);
	layout_->top()->setFirst(header_);
	header_->append(nameContainer_);
	nameContainer_->append(visibility_);
	nameContainer_->append(name_);
	header_->append(arguments_);

	layout_->setLeft(true);
	layout_->left()->setFirst(results_);

	layout_->setContent(content_);
}

VMethod::~VMethod()
{
	SAFE_DELETE_ITEM(layout_);

	// These were automatically deleted by layout's destructor
	name_ = NULL;
	visibility_ = NULL;
	header_ = NULL;
	nameContainer_ = NULL;
	content_ = NULL;
	arguments_ = NULL;
	results_ = NULL;
}

void VMethod::determineChildren()
{
	Method* node = static_cast<Method*> (getNode());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout_->setStyle( &style()->border() );

	if (node->stat() == Static::INSTANCE_VARIABLE) name_->setStyle( &style()->name() );
	else name_->setStyle( &style()->nameStatic());

	visibility_->setStyle( &style()->visibility() );
	header_->setStyle( &style()->header() );
	nameContainer_->setStyle( &style()->nameContainer() );
	content_->setStyle( &style()->content() );
	arguments_->setStyle( &style()->arguments() );
	results_->setStyle( &style()->results() );

	// Synchronize containers
	content_->synchronizeWithNodes(node->items()->nodes().toList(), renderer() );
	arguments_->synchronizeWithNodes(node->arguments()->nodes().toList(), renderer() );
	results_->synchronizeWithNodes(node->results()->nodes().toList(), renderer() );
}

void VMethod::updateGeometry(int, int)
{
	if ( hasShape() )
	{
		getShape()->setOffset(layout_->getXOffsetForExternalShape(), layout_->getYOffsetForExternalShape());
		getShape()->setOutterSize(layout_->getOutterWidthForExternalShape(), layout_->getOutterHeightForExternalShape());
	}
	setSize( layout_->size() );
}

bool VMethod::focusChild(FocusTarget location)
{
	return layout_->focusChild(location);
}


}
