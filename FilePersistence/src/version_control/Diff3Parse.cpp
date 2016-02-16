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

#include "Diff3Parse.h"

namespace FilePersistence {

Chunk::Chunk(bool stable, QList<Model::NodeIdType> idListA, QList<Model::NodeIdType> idListB,
						QList<Model::NodeIdType> idListBase) :
	stable_{stable}, noConflicts_{true}, spanA_{idListA}, spanB_{idListB}, spanBase_{idListBase}
{
	if (stable_)
	{
		Q_ASSERT(spanA_ == spanB_ && spanB_ == spanBase_);
		spanMerged_ = spanBase_;
	}
	else
		spanMerged_ = {};
}

QList<Chunk*> Diff3Parse::computeChunks(const QList<Model::NodeIdType> idListA,
																								 const QList<Model::NodeIdType> idListB,
																								 const QList<Model::NodeIdType> idListBase)
{
	auto lcsA = longestCommonSubsequence(idListBase, idListA);
	auto lcsB = longestCommonSubsequence(idListBase, idListB);

	// auto stableA = QSet<Model::NodeIdType>::fromList(lcsA);
	auto stableB = QSet<Model::NodeIdType>::fromList(lcsB);

	// stableIDs are the sable elements of the diff3 parse
	QList<Model::NodeIdType> stableIDs;
	for (auto id : lcsA)
		if (stableB.contains(id)) stableIDs.append(id);

	auto sublistsA = computeSublists(idListA, stableIDs);
	auto sublistsB = computeSublists(idListB, stableIDs);
	auto sublistsBase = computeSublists(idListBase, stableIDs);

	QList<Chunk*> chunks;

	auto iterA = sublistsA.constBegin();
	auto iterB = sublistsB.constBegin();

	bool isStable = false;
	bool lastWasEmpty = false;
	for (auto iterBase = sublistsBase.constBegin(); iterBase != sublistsBase.constEnd(); ++iterBase)
	{
		if (!iterBase->isEmpty() || !iterA->isEmpty() || !iterB->isEmpty())
		{
			if (lastWasEmpty && !chunks.isEmpty())
			{
				auto chunk = chunks.last();
				chunk->spanA_.append(*iterA);
				chunk->spanB_.append(*iterB);
				chunk->spanBase_.append(*iterBase);
				chunk->spanMerged_.append(*iterBase);
			}
			else
				chunks.append(new Chunk{isStable, *iterA, *iterB, *iterBase});
			lastWasEmpty = false;
		}
		else
			lastWasEmpty = true;
		isStable = !isStable;
		++iterA;
		++iterB;
	}
	Q_ASSERT(isStable);

	return chunks;
}

QList<QList<Model::NodeIdType>> Diff3Parse::computeSublists(const QList<Model::NodeIdType> elementIds,
																						  const QList<Model::NodeIdType> stableIDs)
{
	QList<QList<Model::NodeIdType>> chunks;
	QList<Model::NodeIdType> chunk;

	auto elemId = elementIds.constBegin();
	auto stableId = stableIDs.constBegin();
	while (stableId != stableIDs.constEnd())
	{
		// iterate until end of unstable chunk
		while (elemId != elementIds.constEnd() && *elemId != *stableId)
		{
			chunk.append(*elemId);
			++elemId;
		}
		Q_ASSERT(*elemId == *stableId);
		chunks.append(chunk);
		// create chunk with one stable element.
		// We don't know if in other versions there are unstable elements between this stable
		// element and the next.
		chunk = {*stableId};
		++elemId;
		++stableId;
		chunks.append(chunk);
		chunk = {};
	}
	// iterate until end of last unstable chunk
	while (elemId != elementIds.constEnd())
	{
		chunk.append(*elemId);
		++elemId;
	}
	chunks.append(chunk);

	Q_ASSERT(chunks.size() == 2*stableIDs.size() + 1);

	return chunks;
}

QList<Model::NodeIdType> Diff3Parse::longestCommonSubsequence(const QList<Model::NodeIdType> listA,
																			const QList<Model::NodeIdType> listB)
{
	int m = listA.size() + 1;
	int n = listB.size() + 1;

	int** lcsLength = new int*[m];
	for (int i = 0; i < m; ++i)
		lcsLength[i] = new int[n];

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			lcsLength[i][j] = 0;

	for (int i = 1; i < m; ++i)
		for (int j = 1; j < n; ++j)
			if (listA.at(i-1) == listB.at(j-1))
				lcsLength[i][j] = lcsLength[i-1][j-1] + 1;
			else
				lcsLength[i][j] = std::max(lcsLength[i][j-1], lcsLength[i-1][j]);

	QList<Model::NodeIdType> lcs = backtrackLCS(lcsLength, listA, listB, listA.size(), listB.size());

	for (int i = 0; i < m; ++i)
		delete[] lcsLength[i];

	delete[] lcsLength;

	return lcs;
}

QList<Model::NodeIdType> Diff3Parse::backtrackLCS(int** data, const QList<Model::NodeIdType> listA,
															const QList<Model::NodeIdType> listB, int posA, int posB)
{
	if (posA == 0 || posB == 0)
		return QList<Model::NodeIdType>{};
	else if (listA.at(posA-1) == listB.at(posB-1))
		return backtrackLCS(data, listA, listB, posA-1, posB-1)<<listA.at(posA-1);
	else
		if (data[posA][posB-1] > data[posA-1][posB])
			return backtrackLCS(data, listA, listB, posA, posB-1);
		else
			return backtrackLCS(data, listA, listB, posA-1, posB);
}

}
