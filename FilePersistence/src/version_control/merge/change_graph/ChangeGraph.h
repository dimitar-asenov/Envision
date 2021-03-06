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

#include "MergeChange.h"
#include "Dependencies.h"
#include "Conflicts.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class Diff;

class FILEPERSISTENCE_API ChangeGraph
{
	public:

		~ChangeGraph();

		/**
		 * Adds all changes from both diffs to the change graph.
		 *
		 * The \a tree is the tree which the changes will mutate.
		 */
		void init(Diff& diffA, Diff& diffB, GenericTree* tree);

		/**
		 * Inserts the provided changes in the change graph.
		 *
		 * This operation will recompute all dependencies. The changes are assumed to modify the \a tree.
		 */
		void insert(QList<MergeChange*> changes, GenericTree* tree);

		struct LabelData
		{
			QString label_{};
			MergeChange::Branches branches_{};
		};
		using IdToLabelMap = QMultiHash<Model::NodeIdType, LabelData>;

		void relabelChildrenUniquely(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree);

		void applyNonConflictingChanges(GenericTree* tree);

		const QList<MergeChange*>& changes() const;
		QList<MergeChange*> changesForNode(Model::NodeIdType nodeId) const;
		QList<MergeChange*> changesForChildren(Model::NodeIdType nodeId) const;

		QList<MergeChange*> changesInDirectConflict() const;
		QList<MergeChange*> directConflictsOf(MergeChange* change) const;
		QList<MergeChange*> dependenciesOf(MergeChange* change) const;
		QList<MergeChange*> changesDependingOn(MergeChange* change) const;
		bool hasDirectConflicts() const;

		void removeDeleteChangesConflictingWithMoveOrRelabel(MergeChange* rootDeleteChange);

	private:
		// The nodes of the graph
		QList<MergeChange*> changes_;

		// Helper structures to speed up computation
		QMultiHash<Model::NodeIdType, MergeChange*> changesForNode_;
		QMultiHash<Model::NodeIdType, MergeChange*> changesForChildren_;

		Dependencies dependencies_{changes_, changesForNode_, changesForChildren_};
		Conflicts directConflicts_{changes_, changesForNode_, changesForChildren_};

		static const QString NEW_NODES_PERSISTENT_UNIT_NAME;

		void insertSingleChange(MergeChange* change);

		MergeChange* findIdenticalChange(const MergeChange* change) const;

		void removeLabelOnlyChangesInChildren(Model::NodeIdType parentId);
		void removeLabelDependenciesBetweenChildren(Model::NodeIdType parentId);
		void removeLabelConflictsBetweenChildren(Model::NodeIdType parentId);

		void updateTreeLabels(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree);
		void updateLabelsOfChangesTo(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree);

		void createRelabelChanges(Model::NodeIdType nodeId, QString oldLabel, QList<LabelData> newLabels,
										  Model::NodeIdType parentId);
		void splitMoveInOrInsertChangeForSecondLabel(MergeChange* change, LabelData labelOne, LabelData labelTwo);

		/**
		 * Applies all non-conflicting changes that are not in an all-or-nothing dependency chain and returns their count.
		 */
		int applyIndependentNonConflictingChanges(GenericTree* tree);

		void applyChange(GenericTree* tree, MergeChange* change);
		void removeChange(MergeChange* change, bool mayHaveConflicts);
		QList<MergeChange*>::iterator removeChange(QList<MergeChange*>::iterator changeIt, bool mayHaveConflicts);
};

inline const QList<MergeChange*>& ChangeGraph::changes() const { return changes_; }
inline QList<MergeChange*> ChangeGraph::changesForNode(Model::NodeIdType nodeId) const
{ return changesForNode_.values(nodeId); }
inline QList<MergeChange*> ChangeGraph::changesForChildren(Model::NodeIdType nodeId) const
{ return changesForChildren_.values(nodeId); }
inline QList<MergeChange*> ChangeGraph::changesInDirectConflict() const
{ return directConflicts_.allConflictingChanges(); }
inline QList<MergeChange*> ChangeGraph::directConflictsOf(MergeChange* change) const
{ return directConflicts_.conflictsOf(change); }
inline QList<MergeChange*> ChangeGraph::dependenciesOf(MergeChange* change) const
{ return dependencies_.dependenciesOf(change); }
inline QList<MergeChange*> ChangeGraph::changesDependingOn(MergeChange* change) const
{ return dependencies_.changesDependingOn(change); }
inline bool ChangeGraph::hasDirectConflicts() const { return !directConflicts_.isEmpty(); }

}
