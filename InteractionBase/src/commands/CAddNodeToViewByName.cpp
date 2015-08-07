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

#include "ModelBase/src/SymbolMatcher.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "ModelBase/src/model/TreeManager.h"
#include "VisualizationBase/src/items/VViewItemNode.h"
#include "VisualizationBase/src/declarative/DynamicGridFormElement.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"


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
		return new CommandResult(new CommandError("Please specify a node to add"));

	auto currentView = target->scene()->currentViewItem();
	QPoint posToInsert;
	auto layoutCursor = dynamic_cast<Visualization::LayoutCursor*>(cursor.get());
	if (layoutCursor && cursor->owner() == currentView && cursor->type() == Visualization::Cursor::HorizontalCursor)
	{
		posToInsert.setX(layoutCursor->x());
		posToInsert.setY(layoutCursor->y());
	}
	else if (layoutCursor && cursor->owner() == currentView && cursor->type() == Visualization::Cursor::VerticalCursor)
	{
		currentView->insertColumn(layoutCursor->x());
		posToInsert.setX(layoutCursor->x());
	}
	else if (auto grid = dynamic_cast<Visualization::DynamicGridFormElement*>(currentView->currentForm()))
	{
		auto focusedIndex = grid->indexOf(currentView, source);
		if (focusedIndex.x() != -1)
		{
			posToInsert.setX(focusedIndex.x());
			posToInsert.setY(focusedIndex.y());
		}
	}

	QString name = "";
	for (auto part : commandTokens.mid(1))
		name += part;
	auto matches = mostLikelyMatches(name);
	if (matches.size() > 0)
	{
		currentView->insertNode(matches[0].second, posToInsert.x(), posToInsert.y());
		return new CommandResult();
	}
	return new CommandResult(new CommandError("Could not find node with name " + commandTokens[1]));
}

QList<CommandSuggestion*> CAddNodeToViewByName::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QList<CommandSuggestion*> suggestions;

	if (textSoFar.trimmed().startsWith("add ", Qt::CaseInsensitive))
	{
		auto matches = mostLikelyMatches(textSoFar.trimmed().mid(4));
		for (auto match : matches)
			suggestions.append(new CommandSuggestion("add " + match.first, "Add node " + match.first + " to the view"));
	}
	else if (QString("add ").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive))
			suggestions.append(new CommandSuggestion("add ", "Add nodes to the current view"));

	return suggestions;
}

QList<QPair<QString, Model::Node*>> CAddNodeToViewByName::mostLikelyMatches(const QString& nodeName)
{
	QList<QPair<QString, Model::Node*>> matches;
	auto parts = nodeName.split(".");
	QString pattern = "*";
	for (auto part : parts) pattern += part + '*';
	auto matcher = Model::SymbolMatcher(new QRegExp(pattern, Qt::CaseInsensitive, QRegExp::Wildcard));

	for (auto manager : Model::AllTreeManagers::instance().loadedManagers())
		matches.append(findAllMatches(matcher, "", manager->root()));

	//Shorter names usually have less parts to the fully qualified name -> suggest them first
	std::sort(matches.begin(), matches.end(), [](QPair<QString, Model::Node*> first, QPair<QString, Model::Node*> second)
										{ return first.first.length() < second.first.length(); });
	//Limit the number of suggestions
	matches = matches.mid(0, 10);
	return matches;
}

QList<QPair<QString, Model::Node*>> CAddNodeToViewByName::findAllMatches(const Model::SymbolMatcher& matcher,
		QString nameSoFar, Model::Node* root)
{
	QList<QPair<QString, Model::Node*>> result;

	//If it doesn't define a symbol, just pass it on
	if (!root->definesSymbol())
		for (auto child : root->children())
			result.append(findAllMatches(matcher, nameSoFar, child));

	//If it defines a symbol, check if the name matches with our SymbolMatcher
	else if (isSuggestable(root->symbolType()) && root->symbolName().size() > 0)
	{
		auto newNameSoFar = nameSoFar + "." + root->symbolName();
		for (auto child : root->children())
			result.append(findAllMatches(matcher, newNameSoFar, child));
		if (matcher.matches(newNameSoFar))
			//Get rid of initial "."
			result.append(QPair<QString, Model::Node*>(newNameSoFar.mid(1), root));
	}
	return result;
}

bool CAddNodeToViewByName::isSuggestable(Model::Node::SymbolTypes symbolType)
{
	return symbolType == Model::Node::METHOD || symbolType == Model::Node::CONTAINER;
}

}
