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
#include "GenericPersistentUnit.h"
#include "GenericTree.h"
#include "GenericNode.h"

namespace FilePersistence {

GenericPersistentUnit::GenericPersistentUnit(GenericTree* tree, QString name, char* data, int dataSize)
: tree_{tree}, name_{name}, data_{data}, dataSize_{dataSize},
  lastNodeIndexInLastChunk_{GenericTree::ALLOCATION_CHUNK_SIZE}
{
	Q_ASSERT(tree_);
}

GenericPersistentUnit::~GenericPersistentUnit()
{
	for (auto c : chunks_) tree_->releaseChunk(c);
	chunks_.clear();

	if (data_) delete [] data_;
	data_ = nullptr;
}

GenericNode* GenericPersistentUnit::nextNode()
{
	++lastNodeIndexInLastChunk_;

	if (chunks_.isEmpty() || lastNodeIndexInLastChunk_ >= GenericTree::ALLOCATION_CHUNK_SIZE)
	{
		// We must get a new chunk
		lastNodeIndexInLastChunk_ = 0;
		chunks_.append( tree_->emptyChunk() );
	}

	return chunks_.last() + lastNodeIndexInLastChunk_;
}

GenericNode* GenericPersistentUnit::newNode()
{
	Q_ASSERT(!data_);
	auto node = nextNode();
	node->reset(this);
	return node;
}

GenericNode* GenericPersistentUnit::newNode(int lineStart, int lineEndEnclusive)
{
	Q_ASSERT(data_);
	Q_ASSERT(lineEndEnclusive < dataSize_);

	auto node = nextNode();
	node->reset(this, data_+lineStart, lineEndEnclusive - lineStart + 1, true);
	return node;
}

GenericNode* GenericPersistentUnit::newNode(const char* data, int dataLength)
{
	Q_ASSERT(!data_);
	auto node = nextNode();
	node->reset(this, data, dataLength, false);
	return node;
}

GenericNode* GenericPersistentUnit::newNode(const GenericNode* nodeToCopy)
{
	Q_ASSERT(!data_);
	auto node = nextNode();
	node->reset(this, nodeToCopy);
	return node;
}

const char* GenericPersistentUnit::setData(const char* data, int dataSize)
{
	Q_ASSERT(!data_);
	Q_ASSERT(data);
	Q_ASSERT(dataSize > 0);
	data_ = new char[dataSize];
	dataSize_ = dataSize;
	memcpy(data_, data, dataSize);
	return data_;
}

GenericNode* GenericPersistentUnit::find(Model::NodeIdType id) const
{
	int chunkSize = GenericTree::ALLOCATION_CHUNK_SIZE;
	int currentChunk = 0;
	for (auto chunk : chunks_)
	{
		if (currentChunk == chunks_.size() - 1)
			chunkSize = lastNodeIndexInLastChunk_;
		for (int j = 0; j < chunkSize; j++)
			if (chunk[j].id() == id)
				return &chunk[j];
		currentChunk++;
	}
	return nullptr;
}

} /* namespace FilePersistence */
