/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "CDiff.h"

#include "ModelBase/src/model/TreeManager.h"

#include "VisualizationBase/src/items/Item.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "FilePersistence/src/version_control/GitRepository.h"

#include "VersionControlUI/src/DiffManager.h"

using namespace Visualization;
using namespace FilePersistence;

namespace VersionControlUI {

CDiff::CDiff() : Command{"diff"} {}

bool CDiff::canInterpret(Visualization::Item*, Visualization::Item* target,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	QString managerName = target->node()->manager()->name();

	QStringList commandTokensCopy = commandTokens;

	// get GitRepository
	QString path{"projects/" + managerName};

	if (GitRepository::repositoryExists(path))
	{
		GitRepository repository{path};

		// if there are no tokens we are unable to interpret the command
		if (commandTokensCopy.isEmpty())
			return false;

		// check that command name starts with the characters of the first token
		if (!name().startsWith(commandTokensCopy.takeFirst()))
			return false;

		// if there is an additional version specified, check that it is a valid commit
		if (!commandTokensCopy.isEmpty() && !repository.isValidRevisionString(commandTokensCopy.takeFirst()))
			return false;

		// check the same if there is a second version specified
		if (!commandTokensCopy.isEmpty() && !repository.isValidRevisionString(commandTokensCopy.takeFirst()))
			return false;

		return true;
	}
	else return false;
}

Interaction::CommandResult* CDiff::execute(Visualization::Item*, Visualization::Item* target,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	auto scene = target->scene();
	scene->clearFocus();
	scene->clearSelection();
	scene->setMainCursor(nullptr);

	QString managerName = target->node()->manager()->name();

	// TODO restrict versions to versionA always be older?
	QString versionA = commandTokens.value(1, "HEAD");
	QString versionB = commandTokens.value(2, FilePersistence::GitRepository::WORKDIR);

	VersionControlUI::DiffManager diffManager{versionA, versionB, managerName, Model::SymbolMatcher{"Class"}};
	diffManager.visualize();

	return new Interaction::CommandResult{};
}

QList<Interaction::CommandSuggestion*> CDiff::suggest(Visualization::Item*, Visualization::Item* target,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QStringList tokensSoFar = textSoFar.split(" ");

	QList<Interaction::CommandSuggestion*> suggestions;
	QString commandName = name();

	// no suggestions for that many tokens
	if (tokensSoFar.size() > 3)
		return {};

	// check that the command name starts with the characters of the first token
	if (name().startsWith(tokensSoFar.takeFirst()))
	{
		// no additional versions specified
		if (tokensSoFar.isEmpty())
		{
			suggestions.append(new Interaction::CommandSuggestion{name(), "diff working directory against head"});
			return suggestions;
		}

		QString firstVersionToken = tokensSoFar.takeFirst();
		QString stringToComplete = firstVersionToken;
		QString suggestCommand = name() + " ";
		QString suggestDescription = "";


		bool secondVersionAvailable = !tokensSoFar.isEmpty();
		if (secondVersionAvailable)
		{
			// use the second version token for completion
			stringToComplete = tokensSoFar.takeFirst();

			// add first version token to suggested command
			suggestCommand += firstVersionToken + " ";

			// analyze the first token
			auto firstTokenSuggestion = commitsWithDescriptionsStartingWith(firstVersionToken, target);

			// if first token is too short throw error
			if (firstVersionToken.length() < GitRepository::getMinPrefixLength())
				suggestDescription = "<i>length of first argument must be at least " +
						QString::number(GitRepository::getMinPrefixLength())+"</i>";
			else if (firstTokenSuggestion.size() > 1)
				suggestDescription = "<i>ambiguous</i>";
			else if (firstTokenSuggestion.size() == 0)
				suggestDescription = "<i>no match</i>";
			else
				// add found description of first token
				suggestDescription = firstTokenSuggestion.first().second;

			// new line
			suggestDescription += "<br>";
		}

		for (auto commitWithDescription : commitsWithDescriptionsStartingWith(stringToComplete, target))
				suggestions.append(new Interaction::CommandSuggestion{suggestCommand + commitWithDescription.first,
																						suggestDescription + commitWithDescription.second});

		return suggestions;
	}
	else return {};
}

QList<QPair<QString, QString>> CDiff::commitsWithDescriptionsStartingWith(QString partialCommitId,
																								  Visualization::Item* target)
{
	QString managerName = target->node()->manager()->name();

	// get GitRepository
	QString path{"projects/" + managerName};

	QList<QPair<QString, QString>> commitsWithDescriptions;
	if (GitRepository::repositoryExists(path))
	{
		GitRepository repository{path};

		// use the bigger of minPrefixLength or length of partialCommit as minimum prefix length
		auto commitPrefixes = unambiguousShortestPrefixesPerString(repository.revisions(),
																					  std::max(repository.getMinPrefixLength(),
																									partialCommitId.length()));

		for (auto rev : commitPrefixes)
			if (rev.startsWith(partialCommitId))
				commitsWithDescriptions.append({rev, repository.getCommitInformation(rev).message_});

		// TODO find clean way to make localBranches and tags also available
		//commitsWithDescriptions.append(repository.localBranches());
		//commitsWithDescriptions.append(repository.tags());
	}
	return commitsWithDescriptions;
}

QStringList CDiff::unambiguousShortestPrefixesPerString(const QStringList& strings, const int minPrefixLength)
{
	QStringList shortestPrefixPerString;
	for (auto str : strings)
		for (int i = minPrefixLength; i < str.length(); i++)
		{
			QString currentPrefix = str.left(i);
			if (strings.filter(QRegularExpression{"^"+currentPrefix+".*"}).size() == 1)
			{
				shortestPrefixPerString.append(currentPrefix);
				break;
			}
		}
	return shortestPrefixPerString;
}


}
