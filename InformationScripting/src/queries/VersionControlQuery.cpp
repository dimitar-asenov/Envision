/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "VersionControlQuery.h"

#include "../query_framework/QueryRegistry.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "FilePersistence/src/version_control/GitRepository.h"
#include "FilePersistence/src/version_control/History.h"

#include "ModelBase/src/nodes/Node.h"
#include "OOModel/src/elements/StatementItem.h"

using namespace FilePersistence;

namespace InformationScripting {

const QStringList VersionControlQuery::COUNT_ARGUMENT_NAMES{"c", "count"};
const QStringList VersionControlQuery::NODE_TYPE_ARGUMENT_NAMES{"t", "type"};

Optional<TupleSet> VersionControlQuery::executeLinear(TupleSet)
{
	Model::TreeManager* treeManager = target()->manager();

	// get GitRepository
	QString path("projects/" + treeManager->name());
	if (!GitRepository::repositoryExists(path)) return {"No repository found"};

	GitRepository repository{path};
	TupleSet result;

	auto revisions = repository.revisions();
	bool considerLocalCommitsOnly = arguments_.isArgumentSet(ArgumentParser::LOCAL_SCOPE_ARGUMENT_NAMES[0]) &&
			arguments_.scope(this) == ArgumentParser::Scope::Local;
	if (considerLocalCommitsOnly)
		revisions = nodeHistory(&repository, revisions[revisions.size()-1], target(), treeManager);

	bool converts = false;
	auto changeArgument = arguments_.argument(COUNT_ARGUMENT_NAMES[0]);
	const int CHANGE_COUNT = changeArgument.toInt(&converts);
	int commitIndexToTake = 0;
	if (converts)
		commitIndexToTake = std::min(revisions.size() - 1, CHANGE_COUNT);
	else if (changeArgument == "all")
		commitIndexToTake = revisions.size() - 1;
	else
		return {"Invalid count argument"};

	auto typeMatcher = Model::SymbolMatcher::guessMatcher(arguments_.argument(NODE_TYPE_ARGUMENT_NAMES[1]));

	for (int i = commitIndexToTake; i > 0; --i)
	{
		QString oldCommitId = revisions[i];
		QString newCommitId = revisions[i - 1];

		Diff diff = repository.diff(newCommitId, oldCommitId);
		auto changes = diff.changes();

		addCommitMetaInformation(result, repository.getCommitInformation(newCommitId));

		for (auto change : changes.values())
		{
			// We ignore structure only changes since a node with a structure only change,
			// has a child which was deleted or added or modified, and this child will appear in the final representation.
			if (change->isFake() || change->onlyStructureChange()) continue;

			auto id = change->nodeId();
			if (auto node = const_cast<Model::Node*>(treeManager->nodeIdMap().node(id)))
			{
				Model::Node* changedNode = nullptr;
				if (auto statement = node->firstAncestorOfType(typeMatcher))
					changedNode = statement;
				else // The node is hopefully higher up in the node hierarchy thus we take it as is.
					changedNode = node;

				if (!considerLocalCommitsOnly || target()->isAncestorOf(changedNode))
					result.add({"change", {{"id", newCommitId}, {"ast", changedNode}}});
			}
		}
	}

	return result;
}

void VersionControlQuery::registerDefaultQueries()
{
	QueryRegistry::registerQuery<VersionControlQuery>("changes");
}

VersionControlQuery::VersionControlQuery(Model::Node* target, QStringList args)
	: LinearQuery{target}, arguments_{{
		{COUNT_ARGUMENT_NAMES, "The amount of revisions to look at", COUNT_ARGUMENT_NAMES[1], "10"},
		{NODE_TYPE_ARGUMENT_NAMES, "The minimum type of the nodes returned", NODE_TYPE_ARGUMENT_NAMES[1], "StatementItem"}
}, args, true}
{}

void VersionControlQuery::addCommitMetaInformation(TupleSet& ts, const CommitMetaData& metadata)
{
		ts.add({"commit", {{"id", metadata.sha1_},
				  {"message", metadata.message_},
				  {"date", metadata.dateTime_.toString("dd.MM.yyyy hh:mm")},
				  {"commiter", metadata.committer_.name_ + " " + metadata.committer_.eMail_},
				  {"author", metadata.author_.name_ + " " + metadata.author_.eMail_}}});
}

QList<QString> VersionControlQuery::nodeHistory(const GitRepository* repository, const QString& startSha1,
																Model::Node* target, Model::TreeManager* headManager)
{
	CommitGraph graph = repository->commitGraph(startSha1, "HEAD");

	Model::NodeIdType targetID = headManager->nodeIdMap().id(target);

	QString targetPath = headManager->name();
	Model::Node* headRoot = headManager->root();
	Model::Node* persistentUnitNode = target->persistentUnitNode();
	if (headRoot != persistentUnitNode)
	{
		Model::NodeIdType persistentUnitNodeID = headManager->nodeIdMap().id(persistentUnitNode);
		targetPath = persistentUnitNodeID.toString();
	}

	History history(targetPath, targetID, &graph, repository);

	return history.relevantCommitsByTime(repository);
}

} /* namespace InformationScripting */
