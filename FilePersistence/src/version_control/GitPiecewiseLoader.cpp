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

// TODO implement more robust parsing

namespace FilePersistence {

GitPiecewiseLoader::GitPiecewiseLoader(std::shared_ptr<GenericTree>& tree,
													const GitRepository* repo, QString revision) :
	PiecewiseLoader(tree), repo_{repo}, revision_{revision}, workDir_{repo->workdirPath()} {}

NodeData GitPiecewiseLoader::loadNodeData(Model::NodeIdType id)
{
	auto idString = id.toString();
	auto result = runSystemCommand("git", {"grep", idString, revision_}, workDir_);

	Q_ASSERT(result.exitCode() == 0);
	Q_ASSERT(!result.stdout().isEmpty());

	NodeData nodeData;
	bool found = false;

	for (auto line : result.stdout())
	{
		auto lineParts = line.split(":");
		// first element is revision
		QString persistentUnit = lineParts[1];
		QString nodeLine = lineParts[2];

		// Is it better to do this check here, or already as part of the grep pattern?
		if (idIsNode(idString, nodeLine)) {
			Q_ASSERT(!found); // Check that there is no more than one such node
			found = true;
			nodeData.persistentUnit_ = persistentUnit;
			nodeData.nodeLine_ = nodeLine;
		}
	}

	Q_ASSERT(found);
	return nodeData;
}

QList<NodeData> GitPiecewiseLoader::loadNodeChildrenData(Model::NodeIdType id)
{
	auto idString = id.toString();
	auto result = runSystemCommand("git", {"grep", idString, revision_}, workDir_);

	Q_ASSERT(result.exitCode() == 0);
	Q_ASSERT(!result.stdout().isEmpty());

	QList<NodeData> children;

	for (auto line : result.stdout())
	{
		auto lineParts = line.split(":");
		// first element is revision
		QString persistentUnit = lineParts[1];
		QString nodeLine = lineParts[2];

		if (idIsParent(idString, nodeLine)) {
			NodeData nodeData;
			nodeData.persistentUnit_ = persistentUnit;
			nodeData.nodeLine_ = nodeLine;
			children.append(nodeData);
		}
	}

	return children;
}

bool GitPiecewiseLoader::idIsParent(const QString& id, const QString& nodeLine)
{
	Q_ASSERT(!id.isEmpty());
	if (isPersistenceUnit(nodeLine)) return false;

	auto brace = nodeLine.indexOf('{');
	Q_ASSERT(brace > 0);

	brace = nodeLine.indexOf('{', brace+id.size());
	Q_ASSERT(brace > 0);

	return nodeLine.midRef(brace).startsWith(id);
}

bool GitPiecewiseLoader::idIsNode(const QString& id, const QString& nodeLine)
{
	Q_ASSERT(!id.isEmpty());
	if (isPersistenceUnit(nodeLine)) return false;

	auto brace = nodeLine.indexOf('{');
	Q_ASSERT(brace > 0);
	return nodeLine.midRef(brace).startsWith(id);
}

bool GitPiecewiseLoader::isPersistenceUnit(const QString& nodeLine)
{
	auto space = nodeLine.indexOf(' ');
	Q_ASSERT(space > 0);
	return nodeLine.midRef(space+1).startsWith(GenericNode::PERSISTENT_UNIT_TYPE);
}

} /* namespace FilePersistence */
