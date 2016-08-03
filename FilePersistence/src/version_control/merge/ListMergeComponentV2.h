/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
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

#include "../../filepersistence_api.h"

#include "MergePipelineComponent.h"
#include "MergeChange.h"
#include "ChangeGraph.h"
#include "../Diff3Parse.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {
class GenericNode;
class ChangeGraph;

class FILEPERSISTENCE_API ListMergeComponentV2 : public MergePipelineComponent
{
	public:
		virtual void run(MergeData& mergeData) override;

		static bool isList(const QString& type);
		static bool isUnorderedList(const QString& type);
		static bool isOrderedList(const QString& type);

	private:

		enum class Phase : int {Initial, RelabelComplete, HolesRemoved};
		Phase phase_{Phase::Initial};
		QList<Model::NodeIdType> listsToMerge_{};

		struct IdPosition{
				Model::NodeIdType id{};
				int baseIndex{};
				int offset{};
				MergeChange::Branches branches{};

				bool operator<(const IdPosition& other) const
				{
					if (baseIndex == other.baseIndex)
						return offset < other.offset;
					else
						return baseIndex < other.baseIndex;
				}

				bool isSamePosition(const IdPosition& other) const
				{
					return baseIndex == other.baseIndex && offset == other.offset;
				}

				bool isSamePositionAndID(const IdPosition& other) const
				{
					return isSamePosition(other) && id == other.id;
				}
		};
		/**
		 * Finds all the lists that we will process in the merge.
		 *
		 * Finds the lists having structure changes.
		 * Assumes Insertion/Deletion of whole list does not happen.
		 */
		QList<Model::NodeIdType> computeListsToMerge(MergeData& mergeData);

		/**
		 * Returns the map of new labels(Integral) of list.
		 * so that all labels are unique.
		 */
		ChangeGraph::IdToLabelMap computeAdjustedIndices(Model::NodeIdType listId, MergeData& mergeData);

		/**
		*	Returns chunks for the given list.
		*/
		QList<Chunk*> listToChunks(Model::NodeIdType listId, MergeData& mergeData);

		/**
		 * Returns list of nodeIds sorted by labels that is used for computing chunks.
		 */
		QList<Model::NodeIdType> nodeListToSortedIdList(const QList<GenericNode*>& list);

		/**
		 * Fills the list with the indices mapped to proper positions relative to Base.
		 */
		void computeOffsetsInBranch(const QList<Model::NodeIdType> base, const QList<Model::NodeIdType> version,
										QList<IdPosition>& list, std::shared_ptr<GenericTree> treeBase,  MergeChange::Branches branch);

		/**
		 * Removes Holes from \a listsToMerge_, makes it continuous and adjusts the CG according to it
		 * It gives fractional indices for the elements that were supposed to move in or inserted in the holes
		 */
		void removeHoles(GenericTree* tree, ChangeGraph& cg);

		/**
		 * For Debugging purpose, prints the list and unapplied changes
		 * To be callled after removing holes, at the end of ListMerge Component
		 */
		void printFinalList(const QList<Model::NodeIdType> lists, GenericTree* tree, ChangeGraph& cg);

		void printLabelMap(ChangeGraph::IdToLabelMap map);

};

}
