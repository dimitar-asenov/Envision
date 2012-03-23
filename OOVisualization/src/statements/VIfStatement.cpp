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
 * VIfStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VIfStatement.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIfStatement, "item")

VIfStatement::VIfStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, IfStatement>(parent, node, style),
	header_(new SequentialLayout(nullptr, &style->header())),
	conditionBackground_(new SequentialLayout(nullptr, &style->condition())),
	condition_(nullptr),
	content_(new SequentialLayout(nullptr, &style->content())),
	thenBranch_(nullptr),
	elseBranch_(nullptr)
{
	layout()->setTop(true);
	layout()->top()->setFirst(header_);
	header_->append(new Static(nullptr, &style->icon()));
	header_->append(conditionBackground_);
	layout()->setContent(content_);
}

VIfStatement::~VIfStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = nullptr;
	conditionBackground_ = nullptr;
	condition_ = nullptr;
	content_ = nullptr;
	thenBranch_ = nullptr;
	elseBranch_ = nullptr;
}

void VIfStatement::determineChildren()
{
	conditionBackground_->synchronizeFirst(condition_, node()->condition());
	content_->synchronizeFirst( thenBranch_, node()->thenBranch(), &style()->thenBranch());
	content_->synchronizeLast( elseBranch_, node()->elseBranch(), &style()->elseBranch());


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header_->setStyle(&style()->header());
	header_->at<Static>(0)->setStyle(&style()->icon());
	conditionBackground_->setStyle( &style()->condition() );
	content_->setStyle(&style()->content());
	if (thenBranch_) thenBranch_->setStyle( &style()->thenBranch() );
	if (elseBranch_) elseBranch_->setStyle( &style()->elseBranch() );
}

}
