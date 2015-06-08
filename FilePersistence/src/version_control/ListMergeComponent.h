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

#pragma once

#include "ConflictPipelineComponent.h"

namespace FilePersistence {

class ListMergeComponent : public ConflictPipelineComponent
{
	public:
		ListMergeComponent(QSet<QString>& conflictTypes, QSet<QString>& listTypes, QSet<QString>& unorderedTypes);
		~ListMergeComponent();
		LinkedChangesTransition run(ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
					QSet<std::shared_ptr<const ChangeDescription> >& conflictingChanges,
					ConflictPairs& conflictPairs, LinkedChangesSet&linkedChangesSet);
	private:

		struct Chunk
		{
				bool stable_{};
				QList<Model::NodeIdType> spanA_;
				QList<Model::NodeIdType> spanB_;
				QList<Model::NodeIdType> spanBase_;

				Chunk(bool stable, QList<Model::NodeIdType> idListA, QList<Model::NodeIdType> idListB,
					  QList<Model::NodeIdType> idListBase);
		};

		struct Position
		{
				bool valid_{};
				Model::NodeIdType predecessor_;

				Position(bool valid, Model::NodeIdType predecessor);
				bool operator ==(const Position &other) const;
				bool operator !=(const Position &other) const;
		};

		struct ChunkMergeResult
		{
				bool valid_{};
				QList<Model::NodeIdType> chunk_;

				ChunkMergeResult(bool valid, QList<Model::NodeIdType> chunk);
		};

		/**
		 * Returns all lists that shall be attempted to be merged by the component.
		 */
		QSet<const GenericNode*> computeListsToMerge(
				ChangeDependencyGraph& cdgA,
				ChangeDependencyGraph& cdgB,
				QSet<std::shared_ptr<const ChangeDescription> >& conflictingChanges,
				ConflictPairs& conflictPairs);
		/**
		 * Given an unstable \a chunk, this method tries to construct the merged version of it.
		 *
		 * The \a idList arguments represent the state of the list in the associated revisions.
		 *
		 * The returned chunk has \a valid_ set to \a false if the chunk could not be merged.
		 */
		static ChunkMergeResult computeMergedChunk(Chunk chunk, ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
														QList<Model::NodeIdType>& idListA, QList<Model::NodeIdType>& idListB,
														QList<Model::NodeIdType>& idListBase);

		/**
		 * Tries to find a unique position for \a elem in \a into that is similar to the position of \a elem in \a from.
		 * Returns a Position \a pos where \a pos.valid = true if and only if such a position could be found and
		 * \a pos.predecessor is the element after which \a elem should be inserted or 0 if elem should be inserted at
		 * the beginning. Such a position can be found if the nearest predecessor and successor of \elem in \a from that
		 * are common in \a into are next to each other and in order in \a into.
		 */
		static Position findPosition(Model::NodeIdType element, QList<Model::NodeIdType> from, QList<Model::NodeIdType> into);

		/**
		 * Takes the merged version of a list and generates all changes needed to bring the base version to the merged
		 * version. New changes are created in \a cdgA.
		 */
		LinkedChangesTransition translateListIntoChanges(Model::NodeIdType listContainerId,
																				QList<Model::NodeIdType>& mergedList,
																				ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
																				LinkedChangesSet& linkedChangesSet);
		/**
		 * Returns a new ChangeDescription that is identical to the argument \a change with the exception that \a nodeB
		 * of the new ChangeDescription has the label \a index.
		 */
		static std::shared_ptr<const ChangeDescription> copyWithNewIndex(
				std::shared_ptr<const ChangeDescription>& change, int index);

		QSet<QString> conflictTypes_;
		QSet<QString> listTypes_;
		QSet<QString> unorderedTypes_;

		QString revisionIdA_;
		QString revisionIdB_;
		QString revisionIdBase_;

		std::shared_ptr<GenericTree> treeA_;
		std::shared_ptr<GenericTree> treeB_;
		std::shared_ptr<GenericTree> treeBase_;

		/**
		 * Inserts \a elem into \a chunk according to \a pos.
		 */
		static void insertAfter(Model::NodeIdType elem, Position pos, QList<Model::NodeIdType>& chunk);

		static QList<Model::NodeIdType> nodeListToIdList(const QList<GenericNode*>& list);

		/**
		 * Computes stable and unstable chunks. This is what's called a diff3 parse in the paper by Khanna, Kunal,
		 * Pierce: A Formal Investigation of Diff3 available at http://www.cis.upenn.edu/~bcpierce/papers/diff3-short.pdf.
		 */
		static QList<Chunk> computeChunks(const QList<Model::NodeIdType> idListA, const QList<Model::NodeIdType> idListB,
													const QList<Model::NodeIdType> idListBase);

		static QList<Model::NodeIdType> backtrackLCS(int** data, const QList<Model::NodeIdType> x,
																	const QList<Model::NodeIdType> y, int posX, int posY);

		static QList<QList<Model::NodeIdType>> computeSublists(const QList<Model::NodeIdType> elementIds,
																				 const QList<Model::NodeIdType> stableIDs);

		static QList<Model::NodeIdType> longestCommonSubsequence(const QList<Model::NodeIdType> listA,
																					const QList<Model::NodeIdType> listB);
};

inline bool ListMergeComponent::Position::operator ==(const Position &other) const
{
	return this->valid_ && other.valid_ && this->predecessor_ == other.predecessor_;
}

inline bool ListMergeComponent::Position::operator !=(const Position &other) const { return !(*this == other); }

inline ListMergeComponent::ChunkMergeResult::ChunkMergeResult(bool valid, QList<Model::NodeIdType> chunk) :
	valid_{valid}, chunk_{chunk} {}

inline ListMergeComponent::Chunk::Chunk(bool stable, QList<Model::NodeIdType> idListA, QList<Model::NodeIdType> idListB,
						QList<Model::NodeIdType> idListBase) :
	stable_{stable}, spanA_{idListA}, spanB_{idListB}, spanBase_{idListBase} {}

inline ListMergeComponent::Position::Position(bool valid, Model::NodeIdType predecessor) :
	valid_{valid}, predecessor_{predecessor} {}

} /* namespace FilePersistence */
