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
    tree_->setPiecewiseLoader(std::shared_ptr<PiecewiseLoader>(this));
}
PiecewiseLoader::~PiecewiseLoader(){}

void PiecewiseLoader::loadAndLinkNode(Model::NodeIdType id)
{
	Q_ASSERT(tree_->piecewiseLoader().get() == this);
	Q_ASSERT(!id.isNull());

	auto newNode = loadNewNode(loadNodeData(id));
	Q_ASSERT(newNode);

	newNode->linkNode();
}

void PiecewiseLoader::loadAndLinkNodeChildren(Model::NodeIdType id)
{
	Q_ASSERT(tree_->piecewiseLoader().get() == this);
	Q_ASSERT(!id.isNull());

	auto childrenData = loadNodeChildrenData(id);
	for (auto& childData : childrenData)
	{
		auto newNode = loadNewNode(childData);
		if (newNode) newNode->linkNode();
	}
}

GenericNode* PiecewiseLoader::loadNewNode(const NodeData& nodeData)
{
	Q_ASSERT(!nodeData.persistentUnit_.isEmpty());
	Q_ASSERT(!nodeData.nodeLine_.isEmpty());

	auto pu = tree_->persistentUnit(nodeData.persistentUnit_);
	if (!pu) pu = &tree_->newPersistentUnit(nodeData.persistentUnit_);
	Q_ASSERT(pu);

	auto data = nodeData.nodeLine_.toUtf8();
	auto node = pu->newNode(data.constData(), data.length()); // Will eagerly load the node's contents

	auto alreadyExistingNode = tree_->find(node->id());
	Q_ASSERT(alreadyExistingNode != node);

	if (alreadyExistingNode)
	{
		Q_ASSERT(alreadyExistingNode->persistentUnit() == pu);
		pu->releaseLastNode(); // Remove the node that we just created as it already exists
		node = nullptr; // There was no new node loaded
	}

	return node;
}

}
