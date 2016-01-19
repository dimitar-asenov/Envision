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

#include "CHistory.h"

#include "ModelBase/src/model/TreeManager.h"

#include "FilePersistence/src/version_control/History.h"

using namespace Visualization;
using namespace FilePersistence;

namespace Interaction {

CHistory::CHistory() : CommandWithFlags{"history", {{"project"}}, true, false}
{
}

CommandResult* CHistory::executeNamed(Visualization::Item* /*source*/, Visualization::Item* target,
												  const std::unique_ptr<Visualization::Cursor>& /*cursor*/,
												  const QString& name, const QStringList& /*attributes*/)
{
	auto scene = target->scene();
	scene->clearFocus();
	scene->setMainCursor(nullptr);

	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// get GitRepository
	QString path("projects/");
	path.append(managerName);
	repository = new GitRepository{path};

	CommitGraph graph = repository->commitGraph(name, "HEAD");

	Model::NodeIdType targetID = headManager->nodeIdMap().id(target->node());

	QString targetPath = managerName;
	Model::Node* headRoot = headManager->root();
	Model::Node* persistentUnitNode = target->node()->persistentUnitNode();
	if (headRoot != persistentUnitNode)
	{
		Model::NodeIdType persistentUnitNodeID = headManager->nodeIdMap().id(persistentUnitNode);
		targetPath = persistentUnitNodeID.toString();
	}

	History history(targetPath, targetID, &graph, repository);

	// Output relevant commits in text form
	std::cout << "History:" << std::endl;
	for (QString sha1 : history.relevantCommitsByTime(repository))
	{
		CommitMetaData info = repository->getCommitInformation(sha1);
		printCommitMetaData(info);
	}

	return new CommandResult{};
}

QStringList CHistory::possibleNames(Visualization::Item* /*source*/, Visualization::Item* target,
												const std::unique_ptr<Visualization::Cursor>& /*cursor*/)
{
	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// get GitRepository
	QString path("projects/");
	path.append(managerName);

	QStringList names;
	if (GitRepository::repositoryExists(path))
	{
		GitRepository repository(path);

		names.append(repository.localBranches());
		names.append(repository.tags());
		names.append(repository.revisions());
	}
	return names;
}

void CHistory::printCommitMetaData(CommitMetaData& data) const
{
	std::cout << "commit " << data.sha1_.toStdString().c_str() << std::endl;
	std::cout << "Author: " << data.committer_.name_.toStdString().c_str() << " "
				 << data.committer_.eMail_.toStdString().c_str() << std::endl;
	std::cout << "Date: " << data.dateTime_.toString().toStdString().c_str() << std::endl;
	std::cout << std::endl;
}

}
