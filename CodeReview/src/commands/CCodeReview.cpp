/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
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

#include "CCodeReview.h"

#include "../CodeReviewManager.h"
#include "../nodes/CommentedNode.h"
#include "../nodes/ReviewComment.h"
#include "../orderings/Orderings.h"
#include "../groupings/UseAnalysisGroupings.h"
#include "../overlays/CodeReviewCommentOverlay.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/model/AllTreeManagers.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/ViewItemManager.h"

#include "VersionControlUI/src/DiffManager.h"

using namespace Visualization;

namespace CodeReview {

CCodeReview::CCodeReview() : Command{"review"} {}

bool CCodeReview::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	if (commandTokens.size() > 0)
		return name() == commandTokens.first();
	return false;
}

Interaction::CommandResult* CCodeReview::execute(Visualization::Item* source, Visualization::Item*,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{

	auto ancestorWithNode = source->findAncestorWithNode();
	Model::TreeManager* headManager = ancestorWithNode->node()->manager();
	QString managerName = headManager->name();

	QString oldRev = commandTokens.at(1);
	QString newRev = commandTokens.at(2);

	VersionControlUI::DiffManager diffManager{managerName, {ancestorWithNode->node()->typeName()}};

	auto reviewViewItem = Visualization::VisualizationManager::instance().mainScene()->
			viewItems()->viewItem("ReviewView");

	if (reviewViewItem)
		Visualization::VisualizationManager::instance().mainScene()->
					viewItems()->removeViewItem(reviewViewItem);

	reviewViewItem = Visualization::VisualizationManager::instance().mainScene()->
				viewItems()->newViewItem("ReviewView");

	reviewViewItem->setMajorAxis(Visualization::GridLayouter::NoMajor);
	reviewViewItem->setZoomLabelsEnabled(false);

	auto diffPairs = diffManager.computeDiffFramesWithOverlays(oldRev, newRev, reviewViewItem);

	auto orderedDiffPairs = CodeReviewManager::orderDiffFrames(UseAnalysisGroupings::useAnalysisGrouping,
																							  Orderings::alphabeticalOrdering, diffPairs);

	for (int i = 0; i < orderedDiffPairs.size(); i++)
		for (int j = 0; j < orderedDiffPairs[i].size(); j++)
		{
			MajorMinorIndex index = {i, j};
			reviewViewItem->insertNode(orderedDiffPairs[i][j], index);
		}

	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(reviewViewItem);

	return new Interaction::CommandResult{};
}

QList<Interaction::CommandSuggestion*> CCodeReview::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	if (name().startsWith(textSoFar))
		return {new Interaction::CommandSuggestion{name()}};
	return {};
}

}
