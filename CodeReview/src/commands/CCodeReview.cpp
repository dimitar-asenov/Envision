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
#include "VersionControlUI/src/commands/CDiff.h"

#include "FilePersistence/src/version_control/GitRepository.h"

using namespace Visualization;

namespace CodeReview {

const QString CCodeReview::SAVE_COMMAND = "save";
const QString CCodeReview::REVIEW_VIEW_PREFIX = "ReviewView";

CCodeReview::CCodeReview() : Command{"review"} {}

bool CCodeReview::canInterpret(Visualization::Item* source, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	auto ancestorWithNode = source->findAncestorWithNode();

	if (!ancestorWithNode) return false;

	QString managerName = ancestorWithNode->node()->manager()->name();

	QStringList commandTokensCopy = commandTokens;

	// get GitRepository
	QString path{"projects/" + managerName};

	if (FilePersistence::GitRepository::repositoryExists(path))
	{
		FilePersistence::GitRepository repository{path};

		// if there are no tokens we are unable to interpret the command
		if (commandTokensCopy.isEmpty())
			return false;

		// check that command name starts with the characters of the first token
		if (!name().startsWith(commandTokensCopy.takeFirst()))
			return false;

		// if there is an additional version specified, check that it is a valid commit
		if (!commandTokensCopy.isEmpty())
		{
			auto token = commandTokensCopy.takeFirst();
			auto scene = source->scene();
			if (token == SAVE_COMMAND && scene->currentViewItem()->name().startsWith(REVIEW_VIEW_PREFIX))
				return true;

			if (!repository.isValidRevisionString(unambigousPrefixPerRevision_.value(token, token)))
				return false;
		}
		// check the same if there is a second version specified
		if (!commandTokensCopy.isEmpty())
		{
			auto token = commandTokensCopy.takeFirst();
			if (!repository.isValidRevisionString(unambigousPrefixPerRevision_.value(token, token)))
				return false;
		}

		return true;
	}
	else return false;
}

Interaction::CommandResult* CCodeReview::execute(Visualization::Item* source, Visualization::Item*,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{

	auto ancestorWithNode = source->findAncestorWithNode();
	Model::TreeManager* headManager = ancestorWithNode->node()->manager();
	QString managerName = headManager->name();

	if (commandTokens.value(1) == SAVE_COMMAND)
	{
		CodeReviewManager::instance().saveReview(source->scene()->currentViewItem()->name().split("_").value(2));
		return new Interaction::CommandResult{};
	}
	QString oldRev = commandTokens.value(1, "HEAD");
	QString newRev = commandTokens.value(2, FilePersistence::GitRepository::WORKDIR);

	// try to get complete sha1 if available
	oldRev = unambigousPrefixPerRevision_.value(oldRev, oldRev);
	newRev = unambigousPrefixPerRevision_.value(newRev, newRev);

	VersionControlUI::DiffManager diffManager{managerName, {Model::SymbolMatcher{"Class"},
																			  Model::SymbolMatcher{"Method"}}};

	auto reviewViewName = REVIEW_VIEW_PREFIX + "_" + oldRev + "_" + newRev;

	auto reviewViewItem = Visualization::VisualizationManager::instance().mainScene()->
			viewItems()->viewItem(reviewViewName);

	if (reviewViewItem)
		Visualization::VisualizationManager::instance().mainScene()->
					viewItems()->removeViewItem(reviewViewItem);

	reviewViewItem = Visualization::VisualizationManager::instance().mainScene()->
				viewItems()->newViewItem(reviewViewName);

	reviewViewItem->setMajorAxis(Visualization::GridLayouter::NoMajor);
	reviewViewItem->setZoomLabelsEnabled(false);

	auto diffFramesAndSetup = diffManager.computeDiffFramesAndOverlays(oldRev, newRev, reviewViewItem);

	auto diffFrames = diffFramesAndSetup.diffFrames_;

	auto orderedDiffFrames = CodeReviewManager::orderDiffFrames(UseAnalysisGroupings::useAnalysisGrouping,
																							  Orderings::alphabeticalOrdering, diffFrames);

	for (int i = 0; i < orderedDiffFrames.size(); i++)
		for (int j = 0; j < orderedDiffFrames[i].size(); j++)
		{
			MajorMinorIndex index = {i, j};
			reviewViewItem->insertNode(orderedDiffFrames[i][j], index);
		}

	auto comments = CodeReviewManager::instance().loadReview(newRev);

	if (comments)
	{
		// recreate comment overlays
		Visualization::VisualizationManager::instance().mainScene()->addPostEventAction(
									  [comments, source, reviewViewItem, headManager, diffFramesAndSetup]()
		{
			for (auto comment : *comments)
			{
				Model::Node* node = nullptr;
				auto managerName = comment->managerName()->get();
				if (managerName == diffFramesAndSetup.diffSetup_.newVersionManager_->managerName())
					node = const_cast<Model::Node*>(diffFramesAndSetup.diffSetup_.newVersionManager_->
																 nodeIdMap().node(comment->nodeId()->get()));
				else if (managerName == diffFramesAndSetup.diffSetup_.oldVersionManager_->managerName())
					node = const_cast<Model::Node*>(diffFramesAndSetup.diffSetup_.oldVersionManager_->
																 nodeIdMap().node(comment->nodeId()->get()));
				if (!node) continue;

				for (auto item : reviewViewItem->findAllVisualizationsOf(node))
				{
					auto overlay = new CodeReviewCommentOverlay{item, comment};
					reviewViewItem->addOverlay(overlay, "CodeReviewComment");
				}
			}
		});
	}

	// switch to the newly created view
	Visualization::VisualizationManager::instance().mainScene()->viewItems()->switchToView(reviewViewItem);

	return new Interaction::CommandResult{};
}

QList<Interaction::CommandSuggestion*> CCodeReview::suggest(Visualization::Item* source, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestorWithNode = source->findAncestorWithNode();

	QStringList tokensSoFar = textSoFar.split(" ");

	QList<Interaction::CommandSuggestion*> suggestions;

	// no suggestions for that many tokens
	if (tokensSoFar.size() > 3)
		return {};

	// check that the command name starts with the characters of the first token
	if (name().startsWith(tokensSoFar.takeFirst()))
	{
		// no additional versions specified
		if (tokensSoFar.isEmpty())
		{
			suggestions.append(new Interaction::CommandSuggestion{name(), "review the diff of working "
																					"directory against head"});
			return suggestions;
		}

		suggestions = VersionControlUI::CDiff::parseVersions(tokensSoFar, name(), ancestorWithNode->node()->manager()->name(),
																			  unambigousPrefixPerRevision_);

		if (SAVE_COMMAND.startsWith(tokensSoFar.first()))
			suggestions.prepend(new Interaction::CommandSuggestion{name() + " " + SAVE_COMMAND, "save current review comments"});

		return suggestions;
	}
	else return {};
}

}
