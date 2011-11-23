/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

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
#include "VisualizationBase/headers/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VLoopStatement, "item")

VLoopStatement::VLoopStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<PanelBorderLayout>, LoopStatement>(parent, node, style),
	header(new SequentialLayout(nullptr, &style->header())),
	conditionBackground(nullptr),
	initStepBackground(nullptr),
	updateStepBackground(nullptr),
	condition(nullptr),
	initStep(nullptr),
	updateStep(nullptr),
	body(nullptr)
{
	layout()->setTop(true);
	layout()->top()->setFirst(header);
	header->append(new Static(nullptr, &style->icon()));
}

VLoopStatement::~VLoopStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	header = nullptr;
	conditionBackground = nullptr;
	initStepBackground = nullptr;
	updateStepBackground = nullptr;
	condition = nullptr;
	initStep = nullptr;
	updateStep = nullptr;
	body = nullptr;
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
		initStep = nullptr;
		initStepBackground = nullptr;
	}

	if (condition && condition->node() != node()->condition())
	{
		header->removeAll(conditionBackground);
		condition = nullptr;
		conditionBackground = nullptr;
	}

	if (updateStep && updateStep->node() != node()->updateStep())
	{
		header->removeAll(updateStepBackground);
		updateStep = nullptr;
		updateStepBackground = nullptr;
	}

	// Create nodes which are present in the model
	if (!initStep && node()->initStep())
	{
		initStepBackground = new SequentialLayout(nullptr, &style()->initStep());
		initStep = renderer()->render(nullptr, node()->initStep());
		initStepBackground->append(initStep);
		header->insert(initStepBackground, 1);
	}

	if (!updateStep && node()->updateStep())
	{
		updateStepBackground = new SequentialLayout(nullptr, &style()->updateStep());
		updateStep = renderer()->render(nullptr, node()->updateStep());
		updateStepBackground->append(updateStep);
		header->append(updateStepBackground);
	}

	if (!condition && node()->condition())
	{
		conditionBackground = new SequentialLayout(nullptr, &style()->condition());
		condition = renderer()->render(nullptr, node()->condition());
		conditionBackground->append(condition);
		header->insert(conditionBackground, (initStep?2:1));
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<Static>(0)->setStyle(&style()->icon());
	if (conditionBackground) conditionBackground->setStyle( &style()->condition() );
	if (initStepBackground) initStepBackground->setStyle( &style()->initStep() );
	if (updateStepBackground) updateStepBackground->setStyle( &style()->updateStep() );
	body->setStyle(&style()->body());
}

}
