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

#include "CSceneHandlerLoad.h"

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "ModelBase/src/model/Model.h"

using namespace Visualization;

namespace Interaction {

bool CSceneHandlerLoad::canInterpret(Item*, Item*, const QStringList& commandTokens)
{
	if (commandTokens.size() == 2 && QString("load").startsWith(commandTokens.first()))
		return matchingProjects(commandTokens.last()).size() == 1;

	return false;
}

CommandResult* CSceneHandlerLoad::execute(Item*, Item*, const QStringList& commandTokens)
{
	Q_ASSERT(commandTokens.size() == 2);
	auto matching = matchingProjects(commandTokens.last());
	Q_ASSERT(matching.size() == 1);

	auto model = new Model::Model();
	model->load(new FilePersistence::SimpleTextFileStore("projects/"), matching.first(), false);

	VisualizationManager::instance().mainScene()->addTopLevelItem( new RootItem(model->root()));
	VisualizationManager::instance().mainScene()->listenToModel(model);

	return new CommandResult();
}

QList<CommandSuggestion*> CSceneHandlerLoad::suggest(Item*, Item*, const QString& textSoFar)
{
	auto trimmed = textSoFar.trimmed();
	auto split = trimmed.split(" ");
	if (split.size() > 2 ) return {};
	if (split.size() > 0 && ! QString("load").startsWith(split.first())) return {};

	auto projectName = split.size() == 2 ? split.last() : "";
	QStringList matchingNames = projectName.isEmpty() ? availableProjectsOnDisk() : matchingProjects(projectName);

	QList<CommandSuggestion*> suggestions;
	for (auto name : matchingNames)
		suggestions.append(new CommandSuggestion("load " + name, "Loads the '" + name +"' project"));

	return suggestions;
}

QStringList CSceneHandlerLoad::commandForms(Item*, Item*, const QString& textSoFar)
{
	QStringList forms;
	if (textSoFar.isEmpty() || QString("load").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
		forms.append("load");
	return forms;
}

QStringList CSceneHandlerLoad::availableProjectsOnDisk()
{
	auto dir = QDir( "projects/" );
	return dir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
}

QStringList CSceneHandlerLoad::matchingProjects(QString projectNameToLookFor)
{
	// Use a pattern like this 'p*r*j*' in order to simplify the search. Note that the first letter must match.
	QString searchPattern = projectNameToLookFor;
	for (int i = searchPattern.size(); i>=1; --i) searchPattern.insert(i, "*");
	auto regExp = QRegExp(searchPattern, Qt::CaseInsensitive, QRegExp::Wildcard);

	QStringList result;
	for (auto s : availableProjectsOnDisk())
		if (regExp.exactMatch(s)) result << s;

	return result;
}

} /* namespace Interaction */
