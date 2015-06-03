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
using LinkedChangesSet = QSet<LinkedChanges>;

/**
 * The main purpose of this class is to enforce the consistency of the linked changes mappings.
 */
class LinkedChangesTransition
{
	public:
		LinkedChangesTransition();

		/**
		 *	Creates a one-to-one transition.
		 */
		LinkedChangesTransition(LinkedChangesSet& linkedChangesSet);

		void insert(LinkedChanges keySet, std::shared_ptr<const ChangeDescription>& change);
		LinkedChangesSet values() const;

	private:
		QHash<LinkedChanges, LinkedChanges> transition_;
};

inline LinkedChangesSet LinkedChangesTransition::values() const
{
	return LinkedChangesSet::fromList(transition_.values());
}

inline uint qHash(const LinkedChanges& linkedChanges, uint seed = 0)
{
	return ::qHash(linkedChanges.get(), seed);
}

} /* namespace FilePersistence */
