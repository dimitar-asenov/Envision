/***********************************************************************************************************************
 * VMethod.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VMethod.h"
#include "OOVisualizationException.h"
#include "icons/MethodIcon.h"

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
	header_( new SequentialLayout(NULL, &style->header()) ),
	icon_(new MethodIcon(NULL, &style->icon())),
	name_(new VText(NULL, node->nameNode(), &style->nameDefault()) ),
	arguments_(new SequentialLayout(NULL, &style->arguments()) ),
	content_( new SequentialLayout(NULL, &style->content()) ),
	results_(new SequentialLayout(NULL, &style->results()) )
{
	layout_->setTop(true);
	layout_->top()->setFirst(header_);
	header_->append(icon_);
	header_->append(name_);
	header_->append(arguments_);

	layout_->setLeft(true);
	layout_->left()->setFirst(results_);

	layout_->setContent(content_);
}

VMethod::~VMethod()
{
	SAFE_DELETE_ITEM(layout_);

	// These were automatically deleted by layout's destructor
	header_ = NULL;
	icon_ = NULL;
	name_ = NULL;
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

	if (node->stat() == Static::INSTANCE_VARIABLE)
	{
		if (node->visibility() == Visibility::DEFAULT) name_->setStyle( &style()->nameDefault());
		else if (node->visibility() == Visibility::PRIVATE) name_->setStyle( &style()->namePrivate());
		else if (node->visibility() == Visibility::PROTECTED) name_->setStyle( &style()->nameProtected());
		else if (node->visibility() == Visibility::PUBLIC) name_->setStyle( &style()->namePublic());
		else throw OOVisualizationException("Unknown visibility type in VMethod::determineChildren");
	}
	else if (node->stat() == Static::CLASS_VARIABLE)
	{
		if (node->visibility() == Visibility::DEFAULT) name_->setStyle( &style()->nameStaticDefault());
		else if (node->visibility() == Visibility::PRIVATE) name_->setStyle( &style()->nameStaticPrivate());
		else if (node->visibility() == Visibility::PROTECTED) name_->setStyle( &style()->nameStaticProtected());
		else if (node->visibility() == Visibility::PUBLIC) name_->setStyle( &style()->nameStaticPublic());
		else throw OOVisualizationException("Unknown visibility type in VMethod::determineChildren");
	}
	else throw OOVisualizationException("Unknown static type in VMethod::determineChildren");

	icon_->setStyle( &style()->icon());
	header_->setStyle( &style()->header() );
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
