/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/
#include "GenericTree.h"

#include "GenericNode.h"
#include "PiecewiseLoader.h"

namespace FilePersistence {

GenericTree::GenericTree(QString name)
	: name_{name}{}

GenericTree::~GenericTree()
{
	persistentUnits_.clear();
	for (auto c : emptyChunks_) delete [] c;
}

const std::shared_ptr<PiecewiseLoader>& GenericTree::piecewiseLoader() const
{
	return piecewiseLoader_;
}

QMultiHash<Model::NodeIdType, GenericNode*>& GenericTree::nodesWithoutParents()
{
	return nodesWithoutParents_;
}

void GenericTree::setPiecewiseLoader(std::shared_ptr<PiecewiseLoader> loader)
{
	Q_ASSERT(!piecewiseLoader_);
	Q_ASSERT(!hasQuickLookupHash_);
	Q_ASSERT(persistentUnits_.isEmpty());
	Q_ASSERT(quickLookupHash_.isEmpty());

	piecewiseLoader_ = loader;
	hasQuickLookupHash_ = true;
}

GenericPersistentUnit& GenericTree::newPersistentUnit(QString name, char* data, int dataSize)
{
	Q_ASSERT(!name.isEmpty());
	Q_ASSERT(!persistentUnits_.contains(name));
	Q_ASSERT(!data || !piecewiseLoader_);

	return *persistentUnits_.insert(name, std::shared_ptr<GenericPersistentUnit>
		{new GenericPersistentUnit{this, name, data, dataSize}}).value();
}

GenericPersistentUnit* GenericTree::persistentUnit(const QString& name, bool partial) const
{
	Q_ASSERT(!name.isEmpty());

	QHash<QString, std::shared_ptr<GenericPersistentUnit>>::const_iterator iter;
	if (partial)
	{
		iter = persistentUnits_.cbegin();
		while (iter != persistentUnits_.cend() && !iter.key().contains(name))
			iter++;
	}
	else
		iter = persistentUnits_.find(name);

	if (iter != persistentUnits_.cend())
		return iter.value().get();
	else
		return nullptr;
}

GenericNode* GenericTree::find(Model::NodeIdType id, bool lazyLoad) const
{
	Q_ASSERT(hasQuickLookupHash_);
	auto node = quickLookupHash_.value(id);

	if (!node && lazyLoad)
		node = piecewiseLoader_->loadAndLinkNode(id);

	return node;
}

bool GenericTree::remove(Model::NodeIdType id, bool recursive)
{
	Q_ASSERT(hasQuickLookupHash_);
	// if we have a piecewise loader, load lazily
	auto node = find(id, piecewiseLoader_ != nullptr);
	Q_ASSERT(node);
	node->remove(recursive);
	Q_ASSERT(!nodesWithoutParents_.contains(node->parentId(), node));
	quickLookupHash_.remove(id);
	return true;
}

GenericNode* GenericTree::emptyChunk()
{
	if (emptyChunks_.isEmpty()) return new GenericNode[ALLOCATION_CHUNK_SIZE];
	else return emptyChunks_.takeLast();
}

void GenericTree::releaseChunk(GenericNode* unusedChunk)
{
	emptyChunks_.append(unusedChunk);
}

GenericNode* GenericTree::root() const
{
	auto rootPU = persistentUnit(name_);
	Q_ASSERT(rootPU);
	auto root = rootPU->nodeWithNullParent();
	Q_ASSERT(root && root->parentId().isNull());
	return root;
}

void GenericTree::buildLookupHash()
{
	Q_ASSERT(!piecewiseLoader_);
	if (!hasQuickLookupHash_)
	{
		hasQuickLookupHash_ = true;

		for (auto pu : persistentUnits())
		{
			QList<GenericNode*> stack{pu->unitRootNode()};
			while (!stack.isEmpty())
			{
				auto node = stack.takeLast();
				if (node->type() != GenericNode::PERSISTENT_UNIT_TYPE)
				{
					quickLookupHash_.insert(node->id(), node);
					stack << node->children();
				}
			}
		}
	}
}

}
