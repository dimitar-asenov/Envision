/***********************************************************************************************************************
 * VLoopStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VLoopStatement.h"

#include "OOModel/headers/statements/LoopStatement.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VLoopStatement)

VLoopStatement::VLoopStatement(Item* parent, OOModel::LoopStatement* node, const VLoopStatementStyle* style) :
	ModelItem(parent, node, style),
	layout(new PanelBorderLayout(this, &style->layout())),
	header(new SequentialLayout(NULL, &style->header())),
	conditionBackground(NULL),
	initStepBackground(NULL),
	updateStepBackground(NULL),
	condition(NULL),
	initStep(NULL),
	updateStep(NULL),
	body(NULL)
{
	layout->setTop(true);
	layout->top()->setFirst(header);
	header->append(new SVGIcon(NULL, &style->icon()));
}

VLoopStatement::~VLoopStatement()
{
	SAFE_DELETE_ITEM(layout);

	// These were automatically deleted by layout's destructor
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
	OOModel::LoopStatement* node = static_cast<OOModel::LoopStatement*> (getNode());

	if (!body)
	{
		body = renderer()->render(NULL, node->body());
		layout->setContent(body);
	}

	// Remove nodes which have changed
	if (initStep && initStep->getNode() != node->initStep())
	{
		header->removeAll(initStepBackground);
		initStep = NULL;
		initStepBackground = NULL;
	}

	if (condition && condition->getNode() != node->condition())
	{
		header->removeAll(conditionBackground);
		condition = NULL;
		conditionBackground = NULL;
	}

	if (updateStep && updateStep->getNode() != node->updateStep())
	{
		header->removeAll(updateStepBackground);
		updateStep = NULL;
		updateStepBackground = NULL;
	}

	// Create nodes which are present in the model
	if (!initStep && node->initStep())
	{
		initStepBackground = new SequentialLayout(NULL, &style()->initStep());
		initStep = renderer()->render(NULL, node->initStep());
		initStepBackground->append(initStep);
		header->insert(initStepBackground, 1);
	}

	if (!updateStep && node->updateStep())
	{
		updateStepBackground = new SequentialLayout(NULL, &style()->updateStep());
		updateStep = renderer()->render(NULL, node->updateStep());
		updateStepBackground->append(updateStep);
		header->append(updateStepBackground);
	}

	if (!condition && node->condition())
	{
		conditionBackground = new SequentialLayout(NULL, &style()->condition());
		condition = renderer()->render(NULL, node->condition());
		conditionBackground->append(condition);
		header->insert(conditionBackground, (initStep?2:1));
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<SVGIcon>(0)->setStyle(&style()->icon());
	if (conditionBackground) conditionBackground->setStyle( &style()->condition() );
	if (initStepBackground) initStepBackground->setStyle( &style()->initStep() );
	if (updateStepBackground) updateStepBackground->setStyle( &style()->updateStep() );
}

void VLoopStatement::updateGeometry(int, int)
{
	if ( hasShape() )
	{
		getShape()->setOffset(layout->getXOffsetForExternalShape(), layout->getYOffsetForExternalShape());
		getShape()->setOutterSize(layout->getOutterWidthForExternalShape(), layout->getOutterHeightForExternalShape());
	}
	setSize( layout->size() );
}

bool VLoopStatement::focusChild(FocusTarget location)
{
	return layout->focusChild(location);
}

}
