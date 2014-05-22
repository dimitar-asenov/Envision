/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "CFind.h"

#include "VisualizationBase/src/items/TextRenderer.h"

namespace Interaction {

bool CFind::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens)
{
	return (commandTokens.size() == 1 || commandTokens.size() == 2) && commandTokens.first() == "find";
}

CommandResult* CFind::execute(Visualization::Item*, Visualization::Item* target, const QStringList& commandTokens)
{
	if (commandTokens.size() < 2) return new CommandResult(new CommandError("Please specify a search string"));

	auto scene = target->scene();
	scene->clearFocus();
	scene->clearSelection();
	scene->setMainCursor(nullptr);

	QList<Visualization::Item*> stack = scene->topLevelItems();

	while (!stack.isEmpty())
	{
		auto i = stack.takeLast(); // Depth first exploration
		if (auto text = dynamic_cast<Visualization::TextRenderer*>(i))
		{
			if (text->text().contains(commandTokens.last())) text->setSelected(true);
		}
		else
			stack.append(i->childItems());
	}

	scene->scheduleUpdate();

	return new CommandResult();
}

QList<CommandSuggestion*> CFind::suggest(Visualization::Item*, Visualization::Item*, const QString& textSoFar)
{
	QList<CommandSuggestion*> s;

	if (textSoFar.trimmed().startsWith("find ", Qt::CaseInsensitive))
	{
		auto searchText = textSoFar.trimmed().mid(5);
		s.append(new CommandSuggestion("find " + searchText, "Search for " + searchText));
	}
	else if (QString("find ").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
			s.append(new CommandSuggestion("find ", "Search for text"));

	return s;
}

QStringList CFind::commandForms(Visualization::Item*, Visualization::Item*, const QString& textSoFar)
{
	QStringList forms;
	if (textSoFar.isEmpty() || QString("find").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
		forms.append("find");
	return forms;
}

} /* namespace Interaction */
