/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "GitPiecewiseLoader.h"

#include "GitRepository.h"
#include "../simple/GenericNode.h"

#include "Core/src/global.h"
namespace FilePersistence {

GitPiecewiseLoader::GitPiecewiseLoader(std::shared_ptr<GenericTree>& tree,
													const GitRepository* repo, QString revision) :
	PiecewiseLoader{tree}, repo_{repo}, revision_{revision}, workDir_{repo->workdirPath()} {}

GitPiecewiseLoader::~GitPiecewiseLoader() {}

NodeData GitPiecewiseLoader::loadNodeData(Model::NodeIdType id)
{
	if (!commit_)
		commit_.reset(repo_->getCommit(revision_));
	auto s = commit_->nodeLinesFromId(id, false);

	Q_ASSERT(s.size() == 1 || s.size() == 2);
	for (auto line : s)
		if (!isPersistenceUnit(line))
			return parseGrepLine(line);
	Q_ASSERT(false);
}

QList<NodeData> GitPiecewiseLoader::loadNodeChildrenData(Model::NodeIdType id)
{
	QList<NodeData> children;
	if (!commit_)
		commit_.reset(repo_->getCommit(revision_));

	auto s = commit_->nodeLinesFromId(id, true);
	Q_ASSERT(s.size() == 1);

	for (auto line : s)
		if (!isPersistenceUnit(line))
		{
			auto nodeData = parseGrepLine(line);
			children.append(nodeData);
		}
	return children;
}

NodeData GitPiecewiseLoader::parseGrepLine(const QString& line)
{
	NodeData nodeData;
	nodeData.persistentUnit_ = line.section(':', 0, 0);
	nodeData.nodeLine_ = line.section(':', 1);
	return nodeData;
}

bool GitPiecewiseLoader::isPersistenceUnit(const QString& nodeLine)
{
	auto space = nodeLine.indexOf(' ');
	Q_ASSERT(space > 0);
	return nodeLine.midRef(space+1).startsWith(GenericNode::PERSISTENT_UNIT_TYPE);
}

}
