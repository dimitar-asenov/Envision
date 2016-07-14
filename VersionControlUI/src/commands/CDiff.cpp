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

const QString CDiff::OVERVIEW_COMMAND = "overview";

CDiff::CDiff() : Command{"diff"} {}

bool CDiff::canInterpret(Visualization::Item* source, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	auto ancestorWithNode = source->findAncestorWithNode();

	if (!ancestorWithNode) return false;

	QString managerName = ancestorWithNode->node()->manager()->name();

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
		if (!commandTokensCopy.isEmpty())
		{
			auto token = commandTokensCopy.takeFirst();
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

		// check if next available token is overview command
		if (!commandTokensCopy.isEmpty())
		{
			auto token = commandTokensCopy.takeFirst();
			if (token != OVERVIEW_COMMAND)
				return false;
		}
		return true;
	}
	else return false;
}

Interaction::CommandResult* CDiff::execute(Visualization::Item* source, Visualization::Item*,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& )
{
	auto ancestorWithNode = source->findAncestorWithNode();

	auto scene = ancestorWithNode->scene();
	scene->clearFocus();
	scene->clearSelection();
	scene->setMainCursor(nullptr);

	QString managerName = ancestorWithNode->node()->manager()->name();

	// TODO restrict versions to versionA always be older?
	QString versionA = commandTokens.value(1, "HEAD");
	QString versionB = commandTokens.value(2, FilePersistence::GitRepository::WORKDIR);
	bool highlightChangedParts = commandTokens.size() > 3;

	// try to get complete sha1 if available
	versionA = unambigousPrefixPerRevision_.value(versionA, versionA);
	versionB = unambigousPrefixPerRevision_.value(versionB, versionB);
	QList<Model::SymbolMatcher> symbolMatcherPriorityList;

	symbolMatcherPriorityList.append(Model::SymbolMatcher{"Class"});
	symbolMatcherPriorityList.append(Model::SymbolMatcher{"Method"});

	VersionControlUI::DiffManager diffManager{managerName, symbolMatcherPriorityList};

	if (highlightChangedParts)
		diffManager.highlightChangedParts(versionA, versionB, ancestorWithNode->node()->manager());
	else
		diffManager.showDiff(versionA, versionB);

	return new Interaction::CommandResult{};
}

QString CDiff::descriptionForCommits(QString token, const QList<QPair<QString, QString>>& commits)
{
	QString suggestDescription;

	if (token.length() < GitRepository::getMinPrefixLength())
		suggestDescription = "<i>length of first argument must be at least " +
				QString::number(GitRepository::getMinPrefixLength())+"</i>";
	else if (commits.size() > 1)
		suggestDescription = "<i>ambiguous</i>";
	else if (commits.size() == 0)
		suggestDescription = "<i>no matching commit id</i>";
	else
		// add found description of first token
		suggestDescription = commits.first().second;

	return suggestDescription;
}

QList<Interaction::CommandSuggestion*> CDiff::suggest(Visualization::Item* source, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestorWithNode = source->findAncestorWithNode();

	QStringList tokensSoFar = textSoFar.split(" ");

	QList<Interaction::CommandSuggestion*> suggestions;

	// no suggestions for that many tokens
	if (tokensSoFar.size() > 4)
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

		return parseVersions(tokensSoFar, name(), ancestorWithNode->node()->manager()->name(),
									unambigousPrefixPerRevision_);
	}
	else return {};
}

QList<Interaction::CommandSuggestion*> CDiff::parseVersions(QStringList tokens, QString commandName,
																				QString managerName,
																				QHash<QString, QString>& unambigousPrefixPerRevision)
{
	QList<Interaction::CommandSuggestion*> suggestions;

	QString firstVersionToken = tokens.takeFirst();
	QString stringToComplete = firstVersionToken;
	QString suggestCommand = commandName + " ";
	QString suggestDescription = "";


	bool secondVersionAvailable = !tokens.isEmpty();

	if (secondVersionAvailable)
	{
		// use the second version token for completion
		stringToComplete = tokens.takeFirst();

		// add first version token to suggested command
		suggestCommand += firstVersionToken + " ";

		// analyze the first token
		auto firstTokenSuggestions = commitsWithDescriptionsStartingWith(firstVersionToken, managerName,
																							  unambigousPrefixPerRevision);

		suggestDescription = descriptionForCommits(firstVersionToken, firstTokenSuggestions);

		// new line
		suggestDescription += "<br>";
	}

	// check possible overview command at end
	if (!tokens.isEmpty())
	{
		auto thirdToken = tokens.takeFirst();

		if (!OVERVIEW_COMMAND.startsWith(thirdToken))
			return {};

		suggestCommand += stringToComplete + " ";
		auto secondTokenSuggenstions = commitsWithDescriptionsStartingWith(stringToComplete, managerName,
																								 unambigousPrefixPerRevision);

		suggestDescription += descriptionForCommits(suggestDescription, secondTokenSuggenstions);

		suggestions.append(new Interaction::CommandSuggestion{suggestCommand + OVERVIEW_COMMAND,
																				suggestDescription +
								 "<br> highlight changes in current view"});
		return suggestions;
	}

	for (auto commitWithDescription : commitsWithDescriptionsStartingWith(stringToComplete, managerName,
																								 unambigousPrefixPerRevision))
			suggestions.append(new Interaction::CommandSuggestion{suggestCommand + commitWithDescription.first,
																					suggestDescription + commitWithDescription.second});
	return suggestions;
}

QList<QPair<QString, QString>> CDiff::commitsWithDescriptionsStartingWith(QString partialCommitId,
																							QString managerName,
																							QHash<QString, QString>& unambigousPrefixPerRevision)
{
	// get GitRepository
	QString path{"projects/" + managerName};

	QList<QPair<QString, QString>> commitsWithDescriptions;
	if (GitRepository::repositoryExists(path))
	{
		GitRepository repository{path};

		auto revisions = repository.revisions();

		// use the bigger of minPrefixLength or length of partialCommit as minimum prefix length
		auto prefixes = computeUnambiguousShortestPrefixesPerString(revisions,
																	std::max(repository.getMinPrefixLength(), partialCommitId.length()),
																	unambigousPrefixPerRevision);

		for (auto prefix : prefixes)
			if (prefix.startsWith(partialCommitId))
				commitsWithDescriptions.append({prefix,
														  repository.getCommitInformation(unambigousPrefixPerRevision[prefix]).message_});

		// TODO find clean way to make localBranches and tags also available
		//commitsWithDescriptions.append(repository.localBranches());
		//commitsWithDescriptions.append(repository.tags());
	}
	return commitsWithDescriptions;
}

QStringList CDiff::computeUnambiguousShortestPrefixesPerString(const QStringList& strings, const int minPrefixLength,
																					QHash<QString, QString>& unambigousPrefixPerRevision)
{
	unambigousPrefixPerRevision.clear();
	QStringList prefixes;
	for (auto str : strings)
		for (int i = minPrefixLength; i < str.length(); i++)
		{
			QString currentPrefix = str.left(i);
			if (strings.filter(QRegularExpression{"^"+currentPrefix+".*"}).size() == 1)
			{
				prefixes.append(currentPrefix);
				unambigousPrefixPerRevision.insert(currentPrefix, str);
				break;
			}
		}

	return prefixes;
}

}
