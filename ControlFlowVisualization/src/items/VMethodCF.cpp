/***********************************************************************************************************************
 * VMethodCF.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VMethodCF.h"
#include "ControlFlowVisualizationException.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;
using namespace OOVisualization;
using namespace OOModel;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VMethodCF, "item")

VMethodCF::VMethodCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, Method>(parent, node, style),
	header_( new SequentialLayout(NULL, &style->header()) ),
	icon_(new SVGIcon(NULL, &style->icon())),
	name_(new VText(NULL, node->nameNode(), &style->nameDefault()) ),
	arguments_(new VList(NULL, node->arguments(), &style->arguments()) ),
	content_( NULL ),
	contentCF_(NULL),
	results_(new VList(NULL, node->results(), &style->results()) )
{
	layout()->setTop(true);
	layout()->top()->setFirst(header_);
	header_->append(icon_);
	header_->append(name_);
	header_->append(arguments_);

	layout()->setLeft(true);
	layout()->left()->setFirst(results_);
}

VMethodCF::~VMethodCF()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = NULL;
	icon_ = NULL;
	name_ = NULL;
	content_ = NULL;
	contentCF_ = NULL;
	arguments_ = NULL;
	results_ = NULL;
}

void VMethodCF::determineChildren()
{
	const TextStyle* nameStyle = NULL;
	if (node()->stat() == Static::INSTANCE_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) nameStyle = &style()->nameDefault();
		else if (node()->visibility() == Visibility::PRIVATE) nameStyle = &style()->namePrivate();
		else if (node()->visibility() == Visibility::PROTECTED) nameStyle = &style()->nameProtected();
		else if (node()->visibility() == Visibility::PUBLIC) nameStyle = &style()->namePublic();
		else throw ControlFlowVisualizationException("Unknown visibility type in VMethodCF::determineChildren");
	}
	else if (node()->stat() == Static::CLASS_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) nameStyle = &style()->nameStaticDefault();
		else if (node()->visibility() == Visibility::PRIVATE)nameStyle = &style()->nameStaticPrivate();
		else if (node()->visibility() == Visibility::PROTECTED) nameStyle = &style()->nameStaticProtected();
		else if (node()->visibility() == Visibility::PUBLIC) nameStyle = &style()->nameStaticPublic();
		else throw ControlFlowVisualizationException("Unknown visibility type in VMethodCF::determineChildren");
	}
	else throw ControlFlowVisualizationException("Unknown static type in VMethodCF::determineChildren");

	header_->synchronizeMid(name_, node()->nameNode(), nameStyle, 1);
	header_->synchronizeLast(arguments_, node()->arguments(), &style()->arguments());
	layout()->left()->synchronizeFirst(results_, node()->results(), &style()->results());

	if (style()->showAsControlFlow())
	{
		layout()->synchronizeContent(contentCF_, node()->items(), &style()->contentCF());
		content_ = NULL;
	}
	else
	{
		layout()->synchronizeContent(content_, node()->items(), &style()->content());
		contentCF_ = NULL;
	}


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );
	icon_->setStyle( &style()->icon());
	header_->setStyle( &style()->header() );
	name_->setStyle(nameStyle);
	if (content_) content_->setStyle( &style()->content() );
	if (contentCF_) contentCF_->setStyle( &style()->contentCF() );
	arguments_->setStyle( &style()->arguments() );
	results_->setStyle( &style()->results() );
}

}
