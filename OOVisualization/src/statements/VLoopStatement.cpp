/***********************************************************************************************************************
 * VLoopStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VLoopStatement.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VLoopStatement)

VLoopStatement::VLoopStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<PanelBorderLayout>, LoopStatement>(parent, node, style),
	header(new SequentialLayout(NULL, &style->header())),
	conditionBackground(NULL),
	initStepBackground(NULL),
	updateStepBackground(NULL),
	condition(NULL),
	initStep(NULL),
	updateStep(NULL),
	body( NULL )
{
	layout()->setTop(true);
	layout()->top()->setFirst(header);
	header->append(new SVGIcon(NULL, &style->icon()));
}

VLoopStatement::~VLoopStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	header = NULL;
	conditionBackground = NULL;
	initStepBackground = NULL;
	updateStepBackground = NULL;
	condition = NULL;
	initStep = NULL;
	updateStep = NULL;
	body = NULL;
}

void VLoopStatement::determineChildren()
{
	layout()->synchronizeContent(body, node()->body(), &style()->body());

	// TODO Find a better way to synchronize the condition, initStep and updateStep. Perhaps use make a new layout for
	// Single items. That would be more of a shape provider.

	// Remove nodes which have changed
	if (initStep && initStep->node() != node()->initStep())
	{
		header->removeAll(initStepBackground);
		initStep = NULL;
		initStepBackground = NULL;
	}

	if (condition && condition->node() != node()->condition())
	{
		header->removeAll(conditionBackground);
		condition = NULL;
		conditionBackground = NULL;
	}

	if (updateStep && updateStep->node() != node()->updateStep())
	{
		header->removeAll(updateStepBackground);
		updateStep = NULL;
		updateStepBackground = NULL;
	}

	// Create nodes which are present in the model
	if (!initStep && node()->initStep())
	{
		initStepBackground = new SequentialLayout(NULL, &style()->initStep());
		initStep = renderer()->render(NULL, node()->initStep());
		initStepBackground->append(initStep);
		header->insert(initStepBackground, 1);
	}

	if (!updateStep && node()->updateStep())
	{
		updateStepBackground = new SequentialLayout(NULL, &style()->updateStep());
		updateStep = renderer()->render(NULL, node()->updateStep());
		updateStepBackground->append(updateStep);
		header->append(updateStepBackground);
	}

	if (!condition && node()->condition())
	{
		conditionBackground = new SequentialLayout(NULL, &style()->condition());
		condition = renderer()->render(NULL, node()->condition());
		conditionBackground->append(condition);
		header->insert(conditionBackground, (initStep?2:1));
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<SVGIcon>(0)->setStyle(&style()->icon());
	if (conditionBackground) conditionBackground->setStyle( &style()->condition() );
	if (initStepBackground) initStepBackground->setStyle( &style()->initStep() );
	if (updateStepBackground) updateStepBackground->setStyle( &style()->updateStep() );
	body->setStyle(&style()->body());
}

}
