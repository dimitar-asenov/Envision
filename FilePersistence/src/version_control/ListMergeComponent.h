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

#include "../filepersistence_api.h"

#include "ConflictPipelineComponent.h"
#include "LinkedChangesTransition.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {
class LinkedChangesSet;
class ConflictPairs;
struct Chunk;
class ChangeDescription;
class ChangeDependencyGraph;
class GenericTree;
class GenericNode;

class FILEPERSISTENCE_API ListMergeComponent : public ConflictPipelineComponent
{
	public:
		ListMergeComponent(QSet<QString>& conflictTypes, QSet<QString>& listTypes, QSet<QString>& unorderedTypes);
		~ListMergeComponent();
		LinkedChangesTransition run(std::shared_ptr<GenericTree>& treeA,
											 std::shared_ptr<GenericTree>& treeB,
											 std::shared_ptr<GenericTree>& treeBase,
											 ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
					QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
					ConflictPairs& conflictPairs, LinkedChangesSet&linkedChangesSet);
	private:
		struct Position
		{
				bool valid_{};
				Model::NodeIdType predecessor_;

				Position(bool valid, Model::NodeIdType predecessor);
				bool operator ==(const Position &other) const;
				bool operator !=(const Position &other) const;
		};

		/**
		 * Returns the IDs of all lists that shall be attempted to be merged by the component.
		 */
		QSet<Model::NodeIdType> computeListsToMerge(
				ChangeDependencyGraph& cdgA,
				ChangeDependencyGraph& cdgB,
				QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
				ConflictPairs& conflictPairs);

		std::shared_ptr<ChangeDescription> getConflictingChange(
				ConflictPairs& conflictPairs,
				std::shared_ptr<ChangeDescription>& change);

		QPair<std::shared_ptr<ChangeDescription>, std::shared_ptr<ChangeDescription>> findBranches(
				ChangeDependencyGraph& cdgA,
				std::shared_ptr<ChangeDescription> first,
				std::shared_ptr<ChangeDescription> second);

		QPair<bool, QSet<Model::NodeIdType>> checkAndGetAllElementIds(
				std::shared_ptr<ChangeDescription> changeA,
				std::shared_ptr<ChangeDescription> changeB);

		/**
		 * Given an unstable \a chunk, this method tries to construct the merged version of it.
		 *
		 * The \a idList arguments represent the state of the list in the associated revisions.
		 *
		 * The returned chunk has \a valid_ set to \a false if the chunk could not be merged.
		 */
		void computeMergedChunk(Chunk* chunk, const Model::NodeIdType containerId,
										ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB);

		/**
		 * Returns false if there was a conflict.
		 */
		bool insertElemsIntoChunk(Chunk* chunk,
													const QList<Model::NodeIdType>& spanBase, const Model::NodeIdType containerId,
													const ChangeDependencyGraph& cdgThis,
													const ChangeDependencyGraph& cdgOther,
													const QList<Model::NodeIdType>& spanThis,
													const QList<Model::NodeIdType>& spanOther, bool branchIsA);

		/**
		 * Tries to find a unique position for \a elem in \a chunk that is similar to the position of \a elem in \a origin.
		 * Returns a Position \a pos where \a pos.valid = true if and only if such a position could be found and
		 * \a pos.predecessor is the element after which \a elem should be inserted or 0 if elem should be inserted at
		 * the beginning. Such a position can be found if the nearest predecessor and successor of \elem in \a origin that
		 * are common in \a chunk are next to each other and in order in \a chunk.
		 */
		static Position findPosition(const Model::NodeIdType& element,
											  const QList<Model::NodeIdType>& origin,
											  const Chunk* chunk);

		void markAsResolved(QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
										ConflictPairs& conflictPairs, std::shared_ptr<ChangeDescription> change,
										ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB);
		bool noConflictingDependencies(ChangeDependencyGraph& cdg,
												 QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
												 std::shared_ptr<ChangeDescription>& change);
		void markDependingAsResolved(ChangeDependencyGraph& cdg,
											  QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
											  ConflictPairs& conflictPairs, std::shared_ptr<ChangeDescription>& change);
		void tryResolve(QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
							 ConflictPairs& conflictPairs, std::shared_ptr<ChangeDescription> change,
							 ChangeDependencyGraph& cdgA);

		/**
		 * Marks all chunks that transitively depend on \a chunk as conflicting. \a chunk itself is not explicitly marked but
		 * implicitly if it transitively depends on itself.
		 *
		 * TODO rename to reflect that chunk is also marked
		 */
		void markDependingAsConflicting(Chunk* chunk);


		static bool elementIsStable(const Model::NodeIdType& elem,
									const QList<Model::NodeIdType>& listA,
									const QList<Model::NodeIdType>& listB);

		/**
		 * Returns the chunk of the list of \a listContainer that contains \a element in the base version if
		 * \a listContainer is the id of a prepared list and \a nullptr otherwise.
		 *
		 * We treat a return value of \a nullptr like a chunk that is conflict free.
		 */
		Chunk* findOriginalChunk(Model::NodeIdType element, Model::NodeIdType listContainer, Chunk* guess = nullptr);

		/**
		 * This maps list container IDs to their lists, partitioned into chunks.
		 */
		QHash<Model::NodeIdType, QList<Chunk*>> preparedLists_;

		/**
		 * These nodes have been reordered.
		 */
		QSet<Model::NodeIdType> reorderedNodesA_;
		QSet<Model::NodeIdType> reorderedNodesB_;
		/**
		 * These nodes must not be reordered by the respective branch.
		 * If they are, the mapped chunk must be marked as conflicting.
		 */
		QHash<Model::NodeIdType, Chunk*> mustBeUnchangedByA_;
		QHash<Model::NodeIdType, Chunk*> mustBeUnchangedByB_;
		/**
		 * If the key chunk gets marked as conflicting, all mapped chunks must be marked as conflicting as well.
		 */
		QMultiHash<Chunk*, Chunk*> chunkDependencies_;

		QSet<QString> conflictTypes_;
		QSet<QString> listTypes_;
		QSet<QString> unorderedTypes_;

		std::shared_ptr<GenericTree> treeA_;
		std::shared_ptr<GenericTree> treeB_;
		std::shared_ptr<GenericTree> treeBase_;

		/**
		 * Inserts \a elem into \a chunk according to \a pos.
		 */
		static void insertAfter(Model::NodeIdType elem, Position pos, Chunk* chunk);

		static QList<Model::NodeIdType> nodeListToSortedIdList(const QList<GenericNode*>& list);
};

inline ListMergeComponent::Position::Position(bool valid, Model::NodeIdType predecessor) :
	valid_{valid}, predecessor_{predecessor} {}

inline bool ListMergeComponent::Position::operator ==(const Position &other) const
	{ return this->valid_ && other.valid_ && this->predecessor_ == other.predecessor_; }

inline bool ListMergeComponent::Position::operator !=(const Position &other) const
	{ return !(*this == other); }

}
