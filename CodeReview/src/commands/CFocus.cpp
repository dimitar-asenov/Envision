/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "CFocus.h"

#include "../nodes/CommentedNode.h"
#include "../nodes/ReviewComment.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/model/AllTreeManagers.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/overlays/HighlightOverlay.h"

#include "../CodeReviewManager.h"

#include "../overlays/CodeReviewCommentOverlay.h"

using namespace Visualization;

namespace CodeReview {

CFocus::CFocus() : Command{"focus"} {}

bool CFocus::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	if (commandTokens.size() > 0)
		return name() == commandTokens.first();
	return false;
}

Interaction::CommandResult* CFocus::execute(Visualization::Item*, Visualization::Item*,
				const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{

	Visualization::VisualizationManager::instance().mainScene()->removeOverlayGroup("focusOverlay");

	FocusInformation focusInformation;
	auto focusInformationFound = CodeReviewManager::instance().focusInformationForStep(currentStep_, focusInformation);

	if (!focusInformationFound){
		currentStep_ = 0;
		CodeReviewManager::instance().focusInformationForStep(currentStep_, focusInformation);
	}

	auto focusItem = CodeReviewManager::instance().overlayForCommentedNode(focusInformation.node_);

	switch (focusInformation.type_) {
		case FocusInformation::Center:
			Visualization::VisualizationManager::instance().mainView()->
					centerOn(focusItem);
			break;
		case FocusInformation::Highlight:
			auto overlay = new Visualization::HighlightOverlay{focusItem};
			focusItem->addOverlay(overlay, "focusOverlay");
			break;
	}

	currentStep_++;

	return new Interaction::CommandResult{};
}

QList<Interaction::CommandSuggestion*> CFocus::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	if (name().startsWith(textSoFar))
		return {new Interaction::CommandSuggestion{name()}};
	return {};
}

}
