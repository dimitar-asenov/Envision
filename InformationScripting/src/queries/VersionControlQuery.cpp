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

TupleSet VersionControlQuery::executeLinear(TupleSet)
{
	const int CHANGE_COUNT = argument(COUNT_ARGUMENT_NAMES[0]).toInt();
	QHash<Model::Node*, int> changeCount;

	Model::TreeManager* treeManager = target()->manager();
	QString managerName = treeManager->name();

	// get GitRepository
	QString path("projects/");
	path.append(managerName);
	// TODO user error later:
	Q_ASSERT(GitRepository::repositoryExists(path));
	GitRepository repository{path};

	auto revisions = repository.revisions();
	int commitIndexToTake = std::min(revisions.size() - 1, CHANGE_COUNT);
	for (int i = commitIndexToTake; i > 0; --i)
	{
		QSet<Model::Node*> changedNodes;
		QString oldCommitId = revisions[i];
		QString newCommitId = revisions[i - 1];

		Diff diff = repository.diff(newCommitId, oldCommitId);
		auto changes = diff.changes();

		for (auto change : changes.values())
		{
			// We ignore structure only changes since a node with a structure only change,
			// has a child which was deleted or added or modified, and this child will appear in the final representation.
			if (change->isFake() || change->onlyStructureChange()) continue;

			auto id = change->nodeId();
			// FIXME: here we just use the first StatementItem parent we should solve that better later.
			if (auto node = const_cast<Model::Node*>(treeManager->nodeIdMap().node(id)))
			{
				if (auto statement = node->firstAncestorOfType<OOModel::StatementItem>())
					changedNodes.insert(statement);
				else if (auto statement = DCast<OOModel::StatementItem>(node))
					changedNodes.insert(statement);
				else // The node is hopefully higher up in the node hierarchy thus we take it as is.
					changedNodes.insert(node);
			}
		}

		for (auto changedNode : changedNodes)
			if (changedNode) ++changeCount[changedNode];
	}

	TupleSet result;

	for (auto it = changeCount.begin(); it != changeCount.end(); ++it)
		result.add({{"count", it.value()}, {"ast", it.key()}});

	return result;
}

void VersionControlQuery::registerDefaultQueries()
{
	QueryRegistry::instance().registerQueryConstructor("changes", [](Model::Node* target, QStringList args) {
		return new VersionControlQuery(target, args);
	});
}

VersionControlQuery::VersionControlQuery(Model::Node* target, QStringList args)
	: ScopedArgumentQuery{target, {
		{COUNT_ARGUMENT_NAMES, "The amount of revision to look at", COUNT_ARGUMENT_NAMES[1], "10"}
	}, QStringList{"VersionControl"} + args}
{}

} /* namespace InformationScripting */
