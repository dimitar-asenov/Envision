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

#include "VisualizationBase/src/items/ViewItem.h"
#include "ModelBase/src/model/TreeManager.h"
#include "VisualizationBase/src/items/VViewItemNode.h"
#include "VisualizationBase/src/declarative/GridLayoutFormElement.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"


namespace Interaction {

CAddNodeToViewByName::CAddNodeToViewByName() : Command{"add"}{}

bool CAddNodeToViewByName::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() >= 1 && commandTokens.first() == name();
}

CommandResult* CAddNodeToViewByName::execute(Visualization::Item*, Visualization::Item* target,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	if (commandTokens.size() < 2)
		return new CommandResult(new CommandError("Please specify a node to add"));

	auto currentView = target->scene()->currentViewItem();
	QPoint posToInsert;
	auto layoutCursor = dynamic_cast<Visualization::LayoutCursor*>(cursor.get());
	if (cursor->owner() == currentView && cursor->type() == Visualization::Cursor::HorizontalCursor && layoutCursor)
	{
		posToInsert.setX(layoutCursor->x());
		posToInsert.setY(layoutCursor->y());
	}
	else if (cursor->owner() == currentView && cursor->type() == Visualization::Cursor::VerticalCursor && layoutCursor)
	{
		currentView->insertColumn(layoutCursor->x());
		posToInsert.setX(layoutCursor->x());
	}

	auto tokens = commandTokens.mid(1);
	tokens.removeAll(".");
	for (auto manager : Model::AllTreeManagers::instance().loadedManagers())
		if (auto node = findNode(tokens, manager->root()))
		{
			currentView->insertNode(node, posToInsert.x(), posToInsert.y());
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
		auto nodeName = textSoFar.trimmed().mid(4);
		QStringList names;
		for (auto manager : Model::AllTreeManagers::instance().loadedManagers())
			names.append(findNames(nodeName.split("."), manager->root()));

		//Shorter names usually have less parts to the fully qualified name -> suggest them first
		std::sort(names.begin(), names.end(), [](QString first, QString second)
											{ return first.length() < second.length(); });
		//Limit the number of suggestions
		names = names.mid(0, 10);
		for (auto name : names)
			suggestions.append(new CommandSuggestion("add " + name, "Add node " + name + " to the view"));
	}
	else if (QString("add ").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
			suggestions.append(new CommandSuggestion("add ", "Add nodes to the current view"));

	return suggestions;
}

QStringList CAddNodeToViewByName::findNames(QStringList nameParts, Model::Node* root)
{
	QStringList result;

	//If it doesn't define a symbol, just pass it on
	if (!root->definesSymbol())
		for (auto child : root->children())
			result.append(findNames(nameParts, child));

	//If it defines a symbol, and the name matches, take the part of name it matches and search for the rest
	else if (nameParts.size() > 0 && root->symbolName().startsWith(nameParts[0])
			 && isSuggestable(root->symbolType()))
	{
		for (auto child : root->children())
			for (auto name : findNames(nameParts.mid(1), child))
				result.append(root->symbolName() + "." + name);
		if (nameParts.size() == 1)
			result.append(root->symbolName());
	}
	//If we don't have any name left, accept anything which defines a suggestable symbol
	else if (nameParts.size() == 0 && isSuggestable(root->symbolType()))
	{
		for (auto child : root->children())
			for (auto name : findNames(nameParts, child))
				result.append(root->symbolName() + "." + name);
		result.append(root->symbolName());
	}
	return result;
}

Model::Node* CAddNodeToViewByName::findNode(QStringList fullyQualifiedName, Model::Node* root)
{
	QList<Model::Node*> toSearch{root};
	int currentPart = 0;
	while (!toSearch.isEmpty() && currentPart < fullyQualifiedName.size())
	{
		auto current = toSearch.takeLast();
		if (current->definesSymbol() && current->symbolName() == fullyQualifiedName[currentPart]
				&& currentPart == fullyQualifiedName.size() - 1)
			return current;

		//If it doesn't define a symbol, check in the children
		//Else only continue the path if the name fits
		if (!current->definesSymbol())
			toSearch.append(current->children());
		else if (current->symbolName() == fullyQualifiedName[currentPart])
		{
			toSearch.clear();
			toSearch.append(current->children());
			currentPart++;
		}
	}
	return nullptr;
}

bool CAddNodeToViewByName::isSuggestable(Model::Node::SymbolTypes symbolType)
{
	return symbolType == Model::Node::METHOD || symbolType == Model::Node::CONTAINER;
}

}
