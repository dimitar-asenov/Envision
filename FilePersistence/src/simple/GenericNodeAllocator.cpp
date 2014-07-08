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

#include "GenericNodeAllocator.h"
#include "GenericNode.h"

namespace FilePersistence {

GenericNodeAllocator::GenericNodeAllocator()
{}

GenericNodeAllocator::~GenericNodeAllocator()
{
	Q_ASSERT(dataMappings_.isEmpty());
	Q_ASSERT(currentChunkId_ == -1);
	Q_ASSERT(currentChunk_ == nullptr);
	Q_ASSERT(currentNodeInChunk_ == ALLOCATION_CHUNK_SIZE);

	for (auto c : chunks_) delete [] c;
}

void GenericNodeAllocator::endThisLoad()
{
	currentChunkId_ = rewindRootChunkId_.takeLast();
	currentNodeInChunk_ = rewindRootId_.takeLast();
	if (currentChunkId_ >= 0) currentChunk_ = chunks_.at(currentChunkId_);
	else currentChunk_ = nullptr;

	delete [] dataMappings_.takeLast();
}

GenericNode* GenericNodeAllocator::nextNode()
{
	++currentNodeInChunk_;

	if (currentNodeInChunk_ < ALLOCATION_CHUNK_SIZE) return &currentChunk_[currentNodeInChunk_];

	// We must advance lists
	currentNodeInChunk_ = 0;
	++currentChunkId_;

	if (currentChunkId_ < chunks_.size())
	{
		currentChunk_ = chunks_.at(currentChunkId_);
		return &currentChunk_[currentNodeInChunk_];
	}

	// We must create a new list
	Q_ASSERT(currentChunkId_ == chunks_.size());
	currentChunk_ = new GenericNode[ALLOCATION_CHUNK_SIZE];
	chunks_.append(currentChunk_);
	return &currentChunk_[currentNodeInChunk_];
}

GenericNode* GenericNodeAllocator::newRoot(char* data, int lineStart, int lineEndInclusive)
{
	rewindRootChunkId_.append(currentChunkId_);
	rewindRootId_.append(currentNodeInChunk_);
	dataMappings_.append(data);

	return newChild(lineStart, lineEndInclusive);
}

GenericNode* GenericNodeAllocator::newChild(int lineStart, int lineEndInclusive)
{
	auto node = nextNode();
	node->resetForLoading(dataMappings_.last(), lineStart, lineEndInclusive);
	return node;
}

} /* namespace FilePersistence */
