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

#include "ListMergeComponent.h"
#include "ConflictPairs.h"

namespace FilePersistence {

ListMergeComponent::ListMergeComponent(QSet<QString>& listTypes)
{
	listTypes_ = QSet<QString>(listTypes);
}

void ListMergeComponent::run(const std::unique_ptr<GenericTree>&,
								const std::unique_ptr<GenericTree>&,
								const std::unique_ptr<GenericTree>&,
								ChangeDependencyGraph& cdgA,
								ChangeDependencyGraph& cdgB,
								QSet<ChangeDescription*>& conflictingChanges,
								ConflictPairs& conflictPairs)
{
	// Compute lists  to merge
	for (auto change : conflictingChanges)
	{
		if (onlyChildStructure(change) && listTypes_.contains(change->nodeA()->type()))
		{
			// node of list type and one branch only changes child structure
			ConflictPairs::iterator other = conflictPairs.find(change);
			while (other != conflictPairs.end() &&	other.key() == change &&
					 other.value()->id() != change->id()) other++;
			if (other != conflictPairs.end() && onlyChildStructure(other.value()))
			{
				// other branch only changes child structure
				// now gather all element ids
				QSet<Model::NodeIdType> idsToCheck;
				for (auto child : change->nodeA()->children()) {
					idsToCheck.insert(child->id());
				}
				for (auto child : change->nodeB()->children()) {
					idsToCheck.insert(child->id());
				}
				for (auto child : other.value()->nodeB()->children()) {
					idsToCheck.insert(child->id());
				}
				// for all elements, check that conflicts are on label only
				bool mergeOk = true;
				for (Model::NodeIdType id : idsToCheck)
				{
					auto check = [this, conflictPairs, id](ChangeDependencyGraph& cdg) mutable -> bool
					{
						bool mergeOk = true;
						IdToChangeDescriptionHash::const_iterator elemChange1 = cdg.changes().find(id);
						if (elemChange1 != cdg.changes().end() && elemChange1.key() == id &&
							 !onlyLabel(elemChange1.value()))
						{
							// branch A changes more than label
							ConflictPairs::iterator elemChange2 = conflictPairs.find(elemChange1.value());
							while (elemChange2 != conflictPairs.end() && elemChange2.key() == elemChange1.value())
							{
								if (!(elemChange2.value()->id() == id && onlyLabel(elemChange2.value())))
								{
									// unresolvable
									mergeOk = false;
									break;
								}
							}
						}
						return mergeOk;
					};
					mergeOk = check(cdgA) && check(cdgB);
					if (!mergeOk) break;
				}
				if (mergeOk) listsToMerge_.insert(change->nodeA());
			}
		}
	}
}

ListMergeComponent::Chunk::Chunk(bool stable, QList<Model::NodeIdType> headList, QList<Model::NodeIdType> revisionList,
						QList<Model::NodeIdType> baseList)
	: stable_{stable}, head_{headList}, revision_{revisionList}, base_{baseList} {}

QList<ListMergeComponent::Chunk>& ListMergeComponent::computeMergeChunks(const QList<Model::NodeIdType> idListA,
																								 const QList<Model::NodeIdType> idListB,
																								 const QList<Model::NodeIdType> idListBase,
																								 Model::NodeIdType containerId)
{
	QList<Model::NodeIdType> lcsBaseToHead = longestCommonSubsequence(idListBase, idListA);
	QList<Model::NodeIdType> lcsBaseToRevision = longestCommonSubsequence(idListBase, idListB);

	QSet<Model::NodeIdType> baseHead = QSet<Model::NodeIdType>::fromList(lcsBaseToHead);
	QSet<Model::NodeIdType> baseRevision = QSet<Model::NodeIdType>::fromList(lcsBaseToRevision);

	QList<Model::NodeIdType> stableIDs;
	for (Model::NodeIdType id : idListBase)
		if (baseHead.contains(id) && baseRevision.contains(id))
			stableIDs.append(id);

	QList<QList<Model::NodeIdType>> sublistHead = computeSublists(idListA, stableIDs);
	QList<QList<Model::NodeIdType>> sublistRevision = computeSublists(idListB, stableIDs);
	QList<QList<Model::NodeIdType>> sublistBase = computeSublists(idListBase, stableIDs);

	QList<Chunk> chunks;

	QList<QList<Model::NodeIdType>>::const_iterator iterA = sublistHead.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterB = sublistRevision.constBegin();
	QList<QList<Model::NodeIdType>>::const_iterator iterBase;

	bool isStable = false;
	for (iterBase = sublistBase.constBegin(); iterBase != sublistBase.constEnd(); ++iterBase)
	{
		if (!iterBase->isEmpty() || !iterA->isEmpty() || !iterB->isEmpty())
			chunks.append(Chunk(isStable, *iterA, *iterB, *iterBase));
		isStable = !isStable;
		++iterA;
		++iterB;
	}
	Q_ASSERT(isStable);

	auto iterator = mergedLists_.insert(containerId, chunks);

	return iterator.value();
}

QList<QList<Model::NodeIdType>> ListMergeComponent::computeSublists(const QList<Model::NodeIdType> list,
																		 const QList<Model::NodeIdType> stableIDs)
{
	QList<QList<Model::NodeIdType>> chunks;
	QList<Model::NodeIdType> chunk;

	QList<Model::NodeIdType>::const_iterator stableId = stableIDs.begin();
	for (Model::NodeIdType id : list)
	{
		if (stableId != stableIDs.constEnd() && id == *stableId)
		{
			chunks.append(chunk);
			chunk = QList<Model::NodeIdType>();
			chunk.append(id);
			chunks.append(chunk);
			chunk = QList<Model::NodeIdType>();
			stableId++;
		}
		else
			chunk.append(id);
	}
	chunks.append(chunk);

	Q_ASSERT(chunks.size() == 2*stableIDs.size() + 1);

	return chunks;
}

QList<Model::NodeIdType> ListMergeComponent::longestCommonSubsequence(const QList<Model::NodeIdType> listA,
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

QList<Model::NodeIdType> ListMergeComponent::backtrackLCS(int** data, const QList<Model::NodeIdType> listA,
															const QList<Model::NodeIdType> listB, int posA, int posB)
{
	if (posA == 0 || posB == 0)
		return QList<Model::NodeIdType>();
	else if (listA.at(posA-1) == listB.at(posB-1))
		return backtrackLCS(data, listA, listB, posA-1, posB-1)<<listA.at(posA-1);
	else
		if (data[posA][posB-1] > data[posA-1][posB])
			return backtrackLCS(data, listA, listB, posA, posB-1);
		else
			return backtrackLCS(data, listA, listB, posA-1, posB);
}


} /* namespace FilePersistence */
