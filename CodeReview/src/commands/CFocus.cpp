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

#include "../nodes/NodeReviews.h"
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

int CFocus::currentStep_{0};

CFocus::CFocus() : Command{"focus"} {}

QHash<int, CFocus::FocusInformation> CFocus::focusList_;

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
	focusStep({}, {}, {});
	return new Interaction::CommandResult{};
}

bool CFocus::focusStep(Visualization::Item *, QKeySequence, Interaction::ActionRegistry::InputState)
{
	Visualization::VisualizationManager::instance().mainScene()->removeOverlayGroup("focusOverlay");

	auto focusInformations = focusList_.values(currentStep_);

	if (focusInformations.isEmpty()){
		currentStep_ = 0;
		focusInformations = focusList_.values(currentStep_);
	}

	if (focusInformations.isEmpty())
		return false;

	for (auto focusInformation : focusInformations)
	{

		auto focusItem = CodeReviewManager::instance().overlayForNodeReviews(focusInformation.node_);

		if (focusInformation.type_.testFlag(FocusInformation::Center))
		{
			Visualization::VisualizationManager::instance().mainView()->centerOn(focusItem);
		}

		if (focusInformation.type_.testFlag(FocusInformation::Highlight))
		{
			auto overlay = new Visualization::HighlightOverlay{focusItem,
					Visualization::HighlightOverlay::itemStyles().get("focus")};
			focusItem->addOverlay(overlay, "focusOverlay");
		}
	}

	currentStep_++;
	return true;
}

QList<Interaction::CommandSuggestion*> CFocus::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	if (name().startsWith(textSoFar))
		return {new Interaction::CommandSuggestion{name()}};
	return {};
}

CFocus::FocusInformation CFocus::extractFocusInformation(QString line)
{
	QStringList commandTokens = line.split(" ");

	FocusInformation focusInformation;

	while (!commandTokens.isEmpty())
	{
		auto token = commandTokens.takeFirst();

		if (token.startsWith("h"))
			focusInformation.type_ |= FocusInformation::FocusType::Highlight;
		else if (token.startsWith("c"))
			focusInformation.type_ |= FocusInformation::FocusType::Center;
		else
		{
			bool isNumber;
			auto step = token.toInt(&isNumber);

			if (isNumber)
			{
				focusInformation.step_ = step;
				break;
			}
		}

	}

	return focusInformation;
}

void CFocus::loadFocusInformation()
{
	for (NodeReviews* nodeReviews : *CodeReviewManager::instance().nodeReviewsList())
	{
		auto focusInformation = extractFocusInformation(nodeReviews->focusInformation());

		if (!focusInformation.type_.testFlag(FocusInformation::None))
		{
			focusInformation.node_ = nodeReviews;
			focusList_.insertMulti(focusInformation.step_, focusInformation);
		}
	}
}

}
