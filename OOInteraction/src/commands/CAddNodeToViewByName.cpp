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

#include "OOModel/src/declarations/Declaration.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "OOModel/src/declarations/Declaration.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Project.h"

#include "ModelBase/src/model/TreeManager.h"


namespace OOInteraction {

CAddNodeToViewByName::CAddNodeToViewByName() : Command{"add"}{}

bool CAddNodeToViewByName::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() >= 1 && commandTokens.first() == name();
}

Interaction::CommandResult* CAddNodeToViewByName::execute(Visualization::Item* source, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	if (commandTokens.size() < 2)
		return new Interaction::CommandResult(new Interaction::CommandError("Please specify a node to add"));

	if (auto node = findNode(commandTokens[1],
			Model::AllTreeManagers::instance().loadedManagers().first()->root()))
	{
		source->scene()->currentViewItem()->insertNode(node);
		return new Interaction::CommandResult();
	}
	else
		return new Interaction::CommandResult(new Interaction::CommandError(
					"Could not find node with name " + commandTokens[1]));
}

QList<Interaction::CommandSuggestion*> CAddNodeToViewByName::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QList<Interaction::CommandSuggestion*> suggestions;

	if (textSoFar.trimmed().startsWith("add ", Qt::CaseInsensitive))
	{
		auto nodeName = textSoFar.trimmed().mid(4);
		auto names = findNames(nodeName.split("_"),
						Model::AllTreeManagers::instance().loadedManagers().first()->root());
		//Shorter names usually have less parts to the fully qualified name -> suggest them first
		std::sort(names.begin(), names.end(), [](QString first, QString second)
											{ return first.length() < second.length(); });
		//Limit the number of suggestions
		names = names.mid(0, 10);
		for (auto name : names)
			suggestions.append(new Interaction::CommandSuggestion("add " + name,
									"Add node " + name + " to the view"));
	}
	else if (QString("add ").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
			suggestions.append(new Interaction::CommandSuggestion("add ", "Add nodes to the current view"));

	return suggestions;
}

QStringList CAddNodeToViewByName::findNames(QStringList nameParts, Model::Node* root)
{
	auto declaration = DCast<OOModel::Declaration>(root);
	QStringList result;
	//If it isn't a declaration, just pass it on
	if (!declaration)
		for (auto child : root->children())
			result.append(findNames(nameParts, child));
	//If it is a declaration, and the name matches, take the part of name it matches and search for the rest
	else if (nameParts.size() > 0 && declaration->name().startsWith(nameParts[0]))
	{
		for (auto child : root->children())
			for (auto name : findNames(nameParts.mid(1), child))
				result.append(declaration->name() + "_" + name);
		if (nameParts.size() == 1)
			result.append(declaration->name());
	}
	//If we don't have any name, accept any declarations for suggestions
	else if (nameParts.size() == 0 && isSuggestable(declaration))
	{
		for (auto child : root->children())
			for (auto name : findNames(nameParts, child))
				result.append(declaration->name() + "_" + name);
		result.append(declaration->name());
	}
	return result;
}

OOModel::Declaration* CAddNodeToViewByName::findNode(QString fullyQualifiedName, Model::Node* root)
{
	//TODO can't use "." for some reason (command unknown..)
	auto nameParts = fullyQualifiedName.split("_");
	QList<Model::Node*> toSearch{root};
	int currentPart = 0;
	while (!toSearch.isEmpty() && currentPart < nameParts.size())
	{
		auto current = toSearch.takeLast();
		auto declaration = DCast<OOModel::Declaration>(current);
		if (declaration && declaration->name() == nameParts[currentPart]
				&& currentPart == nameParts.size() - 1)
			return declaration;
		//If it is not a declaration -> search in children.
		//Else only continue the path if the name fits
		if (!declaration)
			toSearch.append(current->children());
		else if (declaration->name() == nameParts[currentPart])
		{
			toSearch.clear();
			toSearch.append(current->children());
			currentPart++;
		}
	}
	return nullptr;
}

bool CAddNodeToViewByName::isSuggestable(OOModel::Declaration *declaration)
{
	//We don't want to return all declarations in the suggestions, as many of them will be almost
	//never used (e.g. local variable declarations -> they can still be added, but not with a suggestion)
	return DCast<OOModel::Method>(declaration) || DCast<OOModel::Class>(declaration)
			|| DCast<OOModel::Project>(declaration) || DCast<OOModel::Module>(declaration);
}

}
