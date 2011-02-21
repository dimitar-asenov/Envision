/***********************************************************************************************************************
 * VMethod.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VMethod.h"
#include "OOVisualizationException.h"
#include "icons/MethodIcon.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VMethod)

VMethod::VMethod(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, Method>(parent, node, style),
	header_( new SequentialLayout(NULL, &style->header()) ),
	icon_(new MethodIcon(NULL, &style->icon())),
	name_(new VText(NULL, node->nameNode(), &style->nameDefault()) ),
	arguments_(new VList(NULL, node->arguments(), &style->arguments()) ),
	content_( new VList(NULL, node->items(), &style->content()) ),
	results_(new VList(NULL, node->results(), &style->results()) )
{
	layout()->setTop(true);
	layout()->top()->setFirst(header_);
	header_->append(icon_);
	header_->append(name_);
	header_->append(arguments_);

	layout()->setLeft(true);
	layout()->left()->setFirst(results_);

	layout()->setContent(content_);
}

VMethod::~VMethod()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = NULL;
	icon_ = NULL;
	name_ = NULL;
	content_ = NULL;
	arguments_ = NULL;
	results_ = NULL;
}

void VMethod::determineChildren()
{
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );

	if (node()->stat() == Static::INSTANCE_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) name_->setStyle( &style()->nameDefault());
		else if (node()->visibility() == Visibility::PRIVATE) name_->setStyle( &style()->namePrivate());
		else if (node()->visibility() == Visibility::PROTECTED) name_->setStyle( &style()->nameProtected());
		else if (node()->visibility() == Visibility::PUBLIC) name_->setStyle( &style()->namePublic());
		else throw OOVisualizationException("Unknown visibility type in VMethod::determineChildren");
	}
	else if (node()->stat() == Static::CLASS_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) name_->setStyle( &style()->nameStaticDefault());
		else if (node()->visibility() == Visibility::PRIVATE) name_->setStyle( &style()->nameStaticPrivate());
		else if (node()->visibility() == Visibility::PROTECTED) name_->setStyle( &style()->nameStaticProtected());
		else if (node()->visibility() == Visibility::PUBLIC) name_->setStyle( &style()->nameStaticPublic());
		else throw OOVisualizationException("Unknown visibility type in VMethod::determineChildren");
	}
	else throw OOVisualizationException("Unknown static type in VMethod::determineChildren");

	icon_->setStyle( &style()->icon());
	header_->setStyle( &style()->header() );
	content_->setStyle( &style()->content() );
	arguments_->setStyle( &style()->arguments() );
	results_->setStyle( &style()->results() );
}

}
