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

class FILEPERSISTENCE_API Conflicts
{
	public:

		Conflicts(const QList<MergeChange*>& changes,
			const QMultiHash<Model::NodeIdType, MergeChange*>& changesForNode,
			const QMultiHash<Model::NodeIdType, MergeChange*>& changesForChildren);

		void addDirectConflicts(MergeChange* change);

		void insert(MergeChange* changeA, MergeChange* changeB);
		void remove(MergeChange* changeA, MergeChange* changeB);
		void removeAll(MergeChange* change);
		void copyConflicts(MergeChange* sourceChange, MergeChange* destinationChange);

		bool hasConflicts(MergeChange* change) const;
		QList<MergeChange*> conflictsOf(MergeChange* change) const;
		bool isEmpty() const;
		QList<MergeChange*> allConflictingChanges() const;

	private:

		// References from Change Graph
		const QList<MergeChange*>& changes_;
		const QMultiHash<Model::NodeIdType, MergeChange*>& changesForNode_;
		const QMultiHash<Model::NodeIdType, MergeChange*>& changesForChildren_;

		QMultiHash<MergeChange*, MergeChange*> directConflicts_;

		void addSameNodeConflict(MergeChange* change);
		void addLabelConfict(MergeChange* change);
		void addDeleteNonEmptyTreeConflict(MergeChange* change);
		void addInsertOrMoveToDeletedConflict(MergeChange* change);
};

inline void Conflicts::insert(MergeChange* changeA, MergeChange* changeB)
{
	Q_ASSERT(changeA != changeB);
	Q_ASSERT(!directConflicts_.contains(changeA, changeB));
	directConflicts_.insert(changeA, changeB);
	directConflicts_.insert(changeB, changeA);
}

inline void Conflicts::remove(MergeChange* changeA, MergeChange* changeB)
{
	directConflicts_.remove(changeA, changeB);
	directConflicts_.remove(changeB, changeA);
}

inline bool Conflicts::hasConflicts(MergeChange* change) const { return directConflicts_.contains(change); }
inline QList<MergeChange*> Conflicts::conflictsOf(MergeChange* change) const { return directConflicts_.values(change);}
inline bool Conflicts::isEmpty() const { return directConflicts_.isEmpty(); }
inline QList<MergeChange*> Conflicts::allConflictingChanges() const { return directConflicts_.uniqueKeys(); }
}
