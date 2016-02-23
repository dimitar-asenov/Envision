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

#include "PiecewiseLoader.h"
#include "GenericTree.h"
#include "GenericNode.h"

namespace FilePersistence {

PiecewiseLoader::PiecewiseLoader(std::shared_ptr<GenericTree>& tree) : tree_{tree}
{
	 tree_->setPiecewiseLoader(std::shared_ptr<PiecewiseLoader>{this});
}

PiecewiseLoader::~PiecewiseLoader() {}

GenericNode* PiecewiseLoader::loadAndLinkNode(Model::NodeIdType id)
{
	Q_ASSERT(tree_->piecewiseLoader().get() == this);
	Q_ASSERT(!id.isNull());

	return loadNewNode(loadNodeData(id));
}

void PiecewiseLoader::loadAndLinkNodeChildren(Model::NodeIdType id)
{
	Q_ASSERT(tree_->piecewiseLoader().get() == this);
	Q_ASSERT(!id.isNull());

	auto childrenData = loadNodeChildrenData(id);
	for (auto& childData : childrenData)
		loadNewNode(childData);
}

GenericNode* PiecewiseLoader::loadNewNode(const NodeData& nodeData)
{
	Q_ASSERT(!nodeData.persistentUnit_.isEmpty());
	Q_ASSERT(!nodeData.nodeLine_.isEmpty());

	auto pu = tree_->persistentUnit(nodeData.persistentUnit_);
	if (!pu) pu = &tree_->newPersistentUnit(nodeData.persistentUnit_);
	Q_ASSERT(pu);

	auto data = nodeData.nodeLine_.toUtf8();
	// Will eagerly load the node's contents
	auto pair = pu->newOrExistingNode(data.constData(), data.length());
	auto node = pair.second;
	if (pair.first)
		node->linkNode();

	// SELF-HOSTING: force a dependency which Envision's reference resolution can't detect yet
	(void)(GenericNode*)nullptr;

	return node;
}

}
