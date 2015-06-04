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

namespace FilePersistence {

using LinkedChanges = std::shared_ptr<QSet<std::shared_ptr<const ChangeDescription>>>;
inline LinkedChanges newLinkedChanges() { return std::make_shared<QSet<std::shared_ptr<const ChangeDescription>>>(); }
inline LinkedChanges newLinkedChanges(LinkedChanges changesToCopy) // TODO is there a better way to do this?
{ return std::make_shared<QSet<std::shared_ptr<const ChangeDescription>>>(*changesToCopy); }

using LinkedChangesSet = QSet<LinkedChanges>;

class LinkedChangesTransition
{
	public:
		/**
		 * Creates an empty transition.
		 */
		LinkedChangesTransition();

		/**
		 *	Creates the identity transition, representing no modifications of the change linking.
		 */
		LinkedChangesTransition(LinkedChangesSet& linkedChangesSet);

		/**
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
		void insert(LinkedChanges keySet, std::shared_ptr<const ChangeDescription>& change);
		LinkedChangesSet values() const;

	private:
		QHash<LinkedChanges, LinkedChanges> transition_;
};

/**
 * Returns the new LinkedChangesSet a.k.a. the new state after the transition.
 */
inline LinkedChangesSet LinkedChangesTransition::values() const
{
	return LinkedChangesSet::fromList(transition_.values());
}

inline uint qHash(const LinkedChanges& linkedChanges, uint seed = 0)
{
	return ::qHash(linkedChanges.get(), seed);
}

} /* namespace FilePersistence */
