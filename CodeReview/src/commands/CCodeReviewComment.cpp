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

#include "CCodeReviewComment.h"

#include "../nodes/CommentedNode.h"
#include "../nodes/CommentWithDateNode.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/model/AllTreeManagers.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/VisualizationManager.h"

#include "../CodeReviewManager.h"

#include "../overlays/CodeReviewCommentOverlay.h"

using namespace Visualization;

namespace CodeReview {

CCodeReviewComment::CCodeReviewComment() : Command{"comment"} {}

bool CCodeReviewComment::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	if (commandTokens.size() > 0)
		return name() == commandTokens.first();
	return false;
}

Interaction::CommandResult* CCodeReviewComment::execute(Visualization::Item* source, Visualization::Item*,
				const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{
	for (auto manager : Model::AllTreeManagers::instance().loadedManagers())
	{
		auto id = manager->nodeIdMap().idIfExists(source->node());
		if (!id.isNull())
		{
			auto id = manager->nodeIdMap().id(source->node()).toString();
			auto commentedNode = CodeReviewManager::instance().commentedNode(id);
			auto commentWithDateNode = new CommentWithDateNode{};
			commentedNode->commentWithDateNodes()->append(commentWithDateNode);

			// only create highlight if not already existent
			if (!source->overlay<CodeReviewCommentOverlay>("CodeReviewComment"))
			{
				auto overlay = new CodeReviewCommentOverlay{source, commentedNode};
				source->addOverlay(overlay, "CodeReviewComment");
			}
			break;
		}
	}

	return new Interaction::CommandResult{};
}

QList<Interaction::CommandSuggestion*> CCodeReviewComment::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	if (name().startsWith(textSoFar))
		return {new Interaction::CommandSuggestion{name()}};
	return {};
}

}
