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

#include "QueryRegistry.h"

#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "FilePersistence/src/version_control/GitRepository.h"

#include "ModelBase/src/nodes/Node.h"
#include "OOModel/src/elements/StatementItem.h"

using namespace FilePersistence;

namespace InformationScripting {

const QStringList VersionControlQuery::COUNT_ARGUMENT_NAMES{"c", "count"};

Optional<TupleSet> VersionControlQuery::executeLinear(TupleSet)
{
	const int CHANGE_COUNT = arguments_.argument(COUNT_ARGUMENT_NAMES[0]).toInt();

	Model::TreeManager* treeManager = target()->manager();
	QString managerName = treeManager->name();

	// get GitRepository
	QString path("projects/");
	path.append(managerName);
	if (!GitRepository::repositoryExists(path)) return {"No repository found"};

	GitRepository repository{path};
	TupleSet result;

	auto revisions = repository.revisions();
	int commitIndexToTake = std::min(revisions.size() - 1, CHANGE_COUNT);
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
			// FIXME: here we just use the first StatementItem parent we should solve that better later.
			if (auto node = const_cast<Model::Node*>(treeManager->nodeIdMap().node(id)))
			{
				Model::Node* changedNode = nullptr;
				if (auto statement = node->firstAncestorOfType<OOModel::StatementItem>())
					changedNode = statement;
				else // The node is hopefully higher up in the node hierarchy thus we take it as is.
					changedNode = node;

				result.add({{newCommitId, changedNode}});
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
		{COUNT_ARGUMENT_NAMES, "The amount of revision to look at", COUNT_ARGUMENT_NAMES[1], "10"}
}, args}
{}

void VersionControlQuery::addCommitMetaInformation(TupleSet& ts, const CommitMetaData& metadata)
{
		ts.add({{"commit", metadata.sha1_},
				  {"message", metadata.message_},
				  {"date", metadata.dateTime_.toString("dd.MM.yyyy hh:mm")},
				  {"commiter", metadata.committer_.name_ + " " + metadata.committer_.eMail_},
				  {"author", metadata.author_.name_ + " " + metadata.author_.eMail_}});
}

} /* namespace InformationScripting */
