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

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class MergeChange;
class GenericTree;
class Conflicts;

class FILEPERSISTENCE_API Dependencies
{
	public:
		Dependencies(const QList<MergeChange*>& changes,
			const QMultiHash<Model::NodeIdType, MergeChange*>& changesForNode,
			const QMultiHash<Model::NodeIdType, MergeChange*>& changesForChildren);

		void recomputeAll(GenericTree* tree);
		void removeAll(MergeChange* change);
		void remove(MergeChange* dependentChange, MergeChange* independentChange);
		void insert(MergeChange* dependentChange, MergeChange* independentChange);

		QList<MergeChange*> dependenciesOf(MergeChange* dependentChange) const;
		QList<MergeChange*> changesDependingOn(MergeChange* independentChange) const;
		bool hasDependencies(MergeChange* change) const;

		void copyDependencies(MergeChange* sourceChange, MergeChange* destinationChange);

		/**
		 * Removes the dependencies of some move changes, which are safe to apply, despite depending on other moves.
		 *
		 * For now these are only move changes that both branches perform.
		 *
		 * Returns whether any dependencies were removed
		 */
		bool removeDependenciesForSafeMoveChanges(const Conflicts& directConflicts, GenericTree* tree);

		/**
		 * Scans all changes to detect all-or-nothing dependency chains and removese the depenencies if all
		 * elements are non-conflicting and if they don't depend on external elements.
		 *
		 * Returns true if any dependencies were removed.
		 */
		bool removeDependenciesInsideNonConflictingAtomicCycles(const Conflicts& directConflicts);

	private:
		// References from Change Graph
		const QList<MergeChange*>& changes_;
		const QMultiHash<Model::NodeIdType, MergeChange*>& changesForNode_;
		const QMultiHash<Model::NodeIdType, MergeChange*>& changesForChildren_;

		// The key depends on the values
		QMultiHash<MergeChange*, MergeChange*> dependencies_;

		// The inverse direction of the edges above, the values depend on the key
		QMultiHash<MergeChange*, MergeChange*> reverseDependencies_;

		void addDependencies(MergeChange* change, GenericTree* tree);
		void addParentPresentDependency(MergeChange* change, GenericTree* tree);
		void addChildrenRemovedDependency(MergeChange* change, GenericTree* tree);
		void addLabelDependency(MergeChange* change);
		void addMoveDependency(MergeChange* change, GenericTree* tree);
};

inline void Dependencies::remove(MergeChange* dependentChange, MergeChange* independentChange)
{
	dependencies_.remove(dependentChange, independentChange);
	reverseDependencies_.remove(independentChange, dependentChange);
}

inline void Dependencies::insert(MergeChange* dependentChange, MergeChange* independentChange)
{
	dependencies_.insert(dependentChange, independentChange);
	reverseDependencies_.insert(independentChange, dependentChange);
}

inline bool Dependencies::hasDependencies(MergeChange* change) const { return dependencies_.contains(change); }

inline QList<MergeChange*> Dependencies::dependenciesOf(MergeChange* dependentChange) const
{ return dependencies_.values(dependentChange); }

inline QList<MergeChange*> Dependencies::changesDependingOn(MergeChange* independentChange) const
{ return reverseDependencies_.values(independentChange); }

}
