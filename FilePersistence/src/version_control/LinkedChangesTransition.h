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

#include "ChangeDescription.h"
#include "ChangeDependencyGraph.h"
#include "LinkedChangesSet.h"

namespace FilePersistence {

class FILEPERSISTENCE_API LinkedChangesTransition
{
	public:
		LinkedChangesTransition();
		/**
		 * Creates an empty transition.
		 */
		LinkedChangesTransition(const LinkedChangesSet& linkedChangesSet);

		/**
		 * Creates the identity transition, representing no modifications of the change linking.
		 */
		LinkedChangesTransition(const LinkedChangesSet& linkedChangesSet,
										const ChangeDependencyGraph& cdgA,
										const ChangeDependencyGraph& cdgB);

		/**
		 * Let \a keySet be the return value of \a this.findLinkedChanges(changeId,inBranchA).
		 * When this method returns, \a keySet will be mapped to a LinkedChanges object containing \a change. All preexisting
		 * mappings persist. Details follow.
		 *
		 * If \a keySet is already mapped to a LinkedChanges containing \a change, this is a no-op.
		 * If \a change is not already mapped to by some LinkedChanges and \a keySet is not already mapped to a LinkedChanges
		 * then \a keySet is mapped to a new LinkedChanges containing only \a change.
		 * If \a change is not already mapped to and \a keySet is already mapped to a LinkedChanges object, \a change is
		 * added to that object.
		 * If \a change is already mapped to by some LinkedChanges and \a keySet is not yet mapped to any LinkedChanges,
		 * \a keySet is
		 * mapped to the LinkedChanges containing \a change.
		 * If \a keySet is already mapped to a LinkedChanges \a current and a different LinkedChanges \a other already
		 * contains \a change, \a keySet and all LinkedChanges mapped to \a other are mapped to the union of \a other and
		 * \a current.
		 */
		void insert(Model::NodeIdType oldChangeId, bool oldInBranchA,
						const std::shared_ptr<const ChangeDescription>& change, bool newInBranchA);

		/**
		 * Equivalent to \a insert(oldChangeId, oldInBranchA, change, oldInBranchA).
		 */
		void insert(Model::NodeIdType oldChangeId, bool oldInBranchA,
						const std::shared_ptr<const ChangeDescription>& change);


		/**
		 * Creates and returns a new LinkedChangesSet object that is a deep copy of the state after the transition.
		 * Nodes pointed to by changes in the returned object are allocated in a new GenericTree.
		 */
		const LinkedChangesSet getNewState() const;

	private:
		LinkedChangesSet oldLinkedChangesSet_;
		QHash<const LinkedChanges, LinkedChanges> transition_;
		QSet<const std::shared_ptr<const ChangeDescription>> changesOfBranchA_;
};

inline void LinkedChangesTransition::insert(Model::NodeIdType oldChangeId, bool oldInBranchA,
														  const std::shared_ptr<const ChangeDescription>& change)
{ return insert(oldChangeId, oldInBranchA, change, oldInBranchA); }

}
