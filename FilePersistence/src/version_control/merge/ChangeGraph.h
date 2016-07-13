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

#include "MergeChange.h"

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

		bool hasConflicts() const;

		/**
		 * Inserts the provided changes in the change graph.
		 *
		 * This operation will recompute all dependencies. The changes are assumed to modify the \a tree.
		 */
		void insert(QList<MergeChange*> changes, GenericTree* tree);

		struct LabelData
		{
			QString label_{};
			MergeChange::Branches branch_{};
		};
		using IdToLabelMap = QMultiHash<Model::NodeIdType, LabelData>;

		void relabelChildrenUniquely(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree);

		void applyNonConflictingChanges(GenericTree* tree);

		const QList<MergeChange*>& changes() const;
		QList<MergeChange*> changesForNode(Model::NodeIdType nodeId) const;

	private:
		// The nodes of the graph
		QList<MergeChange*> changes_;

		// The different edge types of the graph
		QMultiHash<MergeChange*, MergeChange*> dependencies_;
		QMultiHash<MergeChange*, MergeChange*> reverseDependencies_; // The opposite direction of the edges above
		QMultiHash<MergeChange*, MergeChange*> directConflicts_;

		// Helper structures to speed up computation
		QMultiHash<Model::NodeIdType, MergeChange*> changesForNode_;
		QMultiHash<Model::NodeIdType, MergeChange*> changesForChildren_;

		static const QString NEW_NODES_PERSISTENT_UNIT_NAME;

		void insertSingleChange(MergeChange* change);

		MergeChange* findIdenticalChange(const MergeChange* change) const;

		void addDirectConflicts(MergeChange* change);
		void addSameNodeConflict(MergeChange* change);
		void addLabelConfict(MergeChange* change);
		void addDeleteNonEmptyTreeConflict(MergeChange* change);
		void addInsertOrMoveToDeletedConflict(MergeChange* change);

		void recomputeAllDependencies(GenericTree* tree);
		void addDependencies(MergeChange* change, GenericTree* tree);
		void addParentPresentDependency(MergeChange* change, GenericTree* tree);
		void addChildrenRemovedDependency(MergeChange* change, GenericTree* tree);
		void addLabelDependency(MergeChange* change);
		void addMoveDependency(MergeChange* change, GenericTree* tree);

		void removeLabelOnlyChangesInChildren(Model::NodeIdType parentId);
		void removeLabelDependenciesBetweenChildren(Model::NodeIdType parentId);
		void removeLabelConflictsBetweenChildren(Model::NodeIdType parentId);

		void updateTreeLabels(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree);
		void updateLabelsOfChangesTo(Model::NodeIdType parentId, IdToLabelMap labelMap, GenericTree* tree);

		void createRelabelChanges(Model::NodeIdType nodeId, QString oldLabel, QList<LabelData> newLabels,
										  Model::NodeIdType parentId);
		void splitMoveChangeForSecondLabel(MergeChange* change, LabelData labelOne, LabelData labelTwo);

		/**
		 * Applies all non-conflicting changes that are not in an all-or-nothing dependency chain and returns their count.
		 */
		int applyIndependentNonConflictingChanges(GenericTree* tree);

		/**
		 * Scans all changes to detect all-or-nothing dependency chains and removese the depenencies if all
		 * elements are non-conflicting and if they don't depend on external elements.
		 *
		 * Returns true if any dependencies were removed.
		 */
		bool removeDependenciesInsideNonConflictingAtomicChangeGroups();

		void applyChange(GenericTree* tree, MergeChange* change);
		void removeChange(MergeChange* change, bool mayHaveConflicts);
		QList<MergeChange*>::iterator removeChange(QList<MergeChange*>::iterator changeIt, bool mayHaveConflicts);
		void removeAllDependencies(MergeChange* change);
};

inline const QList<MergeChange*>& ChangeGraph::changes() const { return changes_; }
inline QList<MergeChange*> ChangeGraph::changesForNode(Model::NodeIdType nodeId) const
{ return changesForNode_.values(nodeId); }

}
