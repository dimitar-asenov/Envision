/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "CAddNodeToViewByName.h"

#include "ModelBase/src/util/NameResolver.h"
#include "VisualizationBase/src/items/VViewItemNode.h"
#include "VisualizationBase/src/declarative/DynamicGridFormElement.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "VisualizationBase/src/items/ViewItem.h"


namespace Interaction {

CAddNodeToViewByName::CAddNodeToViewByName() : Command{"add"}{}

bool CAddNodeToViewByName::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() >= 1 && commandTokens.first() == name();
}

CommandResult* CAddNodeToViewByName::execute(Visualization::Item* source, Visualization::Item* target,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	if (commandTokens.size() < 2)
		return new CommandResult{new CommandError{"Please specify a node to add"}};

	auto currentView = target->scene()->currentViewItem();
	Visualization::MajorMinorIndex indexToInsert;
	auto layoutCursor = dynamic_cast<Visualization::LayoutCursor*>(cursor.get());

	if (layoutCursor && cursor->owner() == currentView && cursor->type() != Visualization::Cursor::BoxCursor)
	{
		if (cursor->type() == Visualization::Cursor::HorizontalCursor
			 && currentView->majorAxis() == Visualization::GridLayouter::ColumnMajor)
		{
			indexToInsert.major_ = layoutCursor->x();
			indexToInsert.minor_ = layoutCursor->y();
		}
		else if (cursor->type() == Visualization::Cursor::VerticalCursor
								 && currentView->majorAxis() != Visualization::GridLayouter::ColumnMajor)
		{
			indexToInsert.major_ = layoutCursor->y();
			indexToInsert.minor_ = layoutCursor->x();
		}
		else if (cursor->type() == Visualization::Cursor::VerticalCursor
					&& currentView->majorAxis() == Visualization::GridLayouter::ColumnMajor)
		{
			currentView->insertMajor(layoutCursor->x());
			indexToInsert.major_ = layoutCursor->x();
		}
		else if (cursor->type() == Visualization::Cursor::HorizontalCursor
					 && currentView->majorAxis() != Visualization::GridLayouter::ColumnMajor)
		{
			currentView->insertMajor(layoutCursor->y());
			indexToInsert.major_ = layoutCursor->y();
		}
	}
	else if (auto grid = dynamic_cast<Visualization::DynamicGridFormElement*>(currentView->currentForm()))
	{
		auto focusedIndex = grid->indexOf(currentView, source);
		if (focusedIndex.x() != -1)
		{
			indexToInsert.major_ = focusedIndex.x();
			indexToInsert.minor_ = focusedIndex.y();
		}
	}

	QString name = "";
	for (auto part : commandTokens.mid(1))
		name += part;
	auto matches = Model::NameResolver::mostLikelyMatches(name, 10);
	if (matches.size() > 0)
	{
		currentView->insertNode(matches[0].second, indexToInsert);
		return new CommandResult{};
	}
	return new CommandResult{new CommandError{"Could not find node with name " + commandTokens[1]}};
}

QList<CommandSuggestion*> CAddNodeToViewByName::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QList<CommandSuggestion*> suggestions;

	if (textSoFar.trimmed().startsWith("add ", Qt::CaseInsensitive))
	{
		auto matches = Model::NameResolver::mostLikelyMatches(textSoFar.trimmed().mid(4), 10);
		for (auto match : matches)
			suggestions.append(new CommandSuggestion{"add " + match.first, "Add node " + match.first + " to the view"});
	}
	else if (QString{"add "}.startsWith(textSoFar.trimmed(), Qt::CaseInsensitive))
			suggestions.append(new CommandSuggestion{"add ", "Add nodes to the current view"});

	return suggestions;
}

}
