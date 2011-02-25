/***********************************************************************************************************************
 * VIfStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VIfStatement.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIfStatement)

VIfStatement::VIfStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, IfStatement>(parent, node, style),
	header(new SequentialLayout(NULL, &style->header())),
	conditionBackground(new SequentialLayout(NULL, &style->condition())),
	condition(NULL),
	content(new SequentialLayout(NULL, &style->content())),
	thenBranch(NULL),
	elseBranch(NULL)
{
	layout()->setTop(true);
	layout()->top()->setFirst(header);
	header->append(new SVGIcon(NULL, &style->icon()));
	header->append(conditionBackground);
	layout()->setContent(content);
}

VIfStatement::~VIfStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	header = NULL;
	conditionBackground = NULL;
	condition = NULL;
	content = NULL;
	thenBranch = NULL;
	elseBranch = NULL;
}

void VIfStatement::determineChildren()
{
	conditionBackground->synchronizeFirst(condition, node()->condition());
	content->synchronizeFirst( thenBranch, node()->thenBranch(), &style()->thenBranch());
	content->synchronizeLast( elseBranch, node()->elseBranch(), &style()->elseBranch());


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<SVGIcon>(0)->setStyle(&style()->icon());
	conditionBackground->setStyle( &style()->condition() );
	content->setStyle(&style()->content());
	if (thenBranch) thenBranch->setStyle( &style()->thenBranch() );
	if (elseBranch) elseBranch->setStyle( &style()->elseBranch() );
}

}
