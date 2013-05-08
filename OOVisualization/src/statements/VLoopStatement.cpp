/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "statements/VLoopStatement.h"

#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VLoopStatement, "item")

VLoopStatement::VLoopStatement(Item* parent, NodeType* node, const StyleType* style) :
	Super(parent, node, style),
	header_(), conditionBackground_(), initStepBackground_(), updateStepBackground_(), condition_(), initStep_(),
	updateStep_(),	body_()
{
	layout()->setTop(true);
	header_ = new SequentialLayout(layout()->top(), &style->header());
	layout()->top()->setFirst(header_);
	header_->append(new Static(header_, &style->icon()));
}

VLoopStatement::~VLoopStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = nullptr;
	conditionBackground_ = nullptr;
	initStepBackground_ = nullptr;
	updateStepBackground_ = nullptr;
	condition_ = nullptr;
	initStep_ = nullptr;
	updateStep_ = nullptr;
	body_ = nullptr;
}

void VLoopStatement::determineChildren()
{
	layout()->synchronizeContent(body_, node()->body(), &style()->body());

	// TODO Find a better way to synchronize the condition, initStep and updateStep. Perhaps use make a new layout for
	// Single items. That would be more of a shape provider.

	// Remove nodes which have changed
	if (initStep_ && initStep_->node() != node()->initStep())
	{
		header_->removeAll(initStepBackground_);
		initStep_ = nullptr;
		initStepBackground_ = nullptr;
	}

	if (condition_ && condition_->node() != node()->condition())
	{
		header_->removeAll(conditionBackground_);
		condition_ = nullptr;
		conditionBackground_ = nullptr;
	}

	if (updateStep_ && updateStep_->node() != node()->updateStep())
	{
		header_->removeAll(updateStepBackground_);
		updateStep_ = nullptr;
		updateStepBackground_ = nullptr;
	}

	// Create nodes which are present in the model
	if (!initStep_ && node()->initStep())
	{
		initStepBackground_ = new SequentialLayout(header_, &style()->initStep());
		initStep_ = renderer()->render(initStepBackground_, node()->initStep());
		initStepBackground_->append(initStep_);
		header_->insert(initStepBackground_, 1);
	}

	if (!updateStep_ && node()->updateStep())
	{
		updateStepBackground_ = new SequentialLayout(header_, &style()->updateStep());
		updateStep_ = renderer()->render(updateStepBackground_, node()->updateStep());
		updateStepBackground_->append(updateStep_);
		header_->append(updateStepBackground_);
	}

	if (!condition_ && node()->condition())
	{
		conditionBackground_ = new SequentialLayout(header_, &style()->condition());
		condition_ = renderer()->render(conditionBackground_, node()->condition());
		conditionBackground_->append(condition_);
		header_->insert(conditionBackground_, (initStep_?2:1));
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header_->setStyle(&style()->header());
	header_->at<Static>(0)->setStyle(&style()->icon());
	if (conditionBackground_) conditionBackground_->setStyle( &style()->condition() );
	if (initStepBackground_) initStepBackground_->setStyle( &style()->initStep() );
	if (updateStepBackground_) updateStepBackground_->setStyle( &style()->updateStep() );
	body_->setStyle(&style()->body());
}

}
