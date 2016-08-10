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

#include "../../../filepersistence_api.h"

#include "MergePipelineComponent.h"
#include "../change_graph/MergeChange.h"

namespace FilePersistence {

class ChangeGraph;

/**
 * The DiscardConflictingDeletesComponent detects subtrees which one branch removes entirely and another alters.
 *
 * There are two allowed cases:
 * - The other branch moves the subtree.
 *
 * - The other branch relabels the subtree. There should be no deletes dependent on the delete.
 *
 * In all such cases, the delete operations are discarded and only the others are left. It's important to run this
 * component before any changes have been removed. Otherwise it could happen that some nodes are deleted, and then
 * the parent nodes are moved by the other branch which is unaware of the deletions and results in a tree with holes.
 */
class FILEPERSISTENCE_API DiscardConflictingDeletesComponent : public MergePipelineComponent
{
	public:
		virtual void run(MergeData& mergeData) override;

	private:
		QList<MergeChange*> deletesConflictingWithMoveOrRelabel(ChangeGraph& cg);
		void computeDeletedSubtrees(ChangeGraph& cg, QList<MergeChange*> deletesToLookAt);

		bool isSubtreeCompletelyDeleted(MergeChange* deleteChange, MergeChange::Branch deletingBranch, ChangeGraph& cg);
		void removeSuitableDeletes(ChangeGraph& cg, QList<MergeChange*> deletesToLookAt);
		bool isAncestorDeleteInList(MergeChange* change, QList<MergeChange*> deletesToLookAt);

		QMultiHash<MergeChange*, MergeChange::Branch> subtreeIsNotDeleted;
		QMultiHash<MergeChange*, MergeChange::Branch> subtreeIsDeleted;
		QHash<MergeChange*, MergeChange*> parentDelete;
};

}
