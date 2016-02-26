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

class ChangeDependencyGraph;
class GenericTree;

using LinkedChanges = std::shared_ptr<QSet<std::shared_ptr<const ChangeDescription>>>;

class FILEPERSISTENCE_API LinkedChangesSet : public QSet<LinkedChanges>
{
	public:
		LinkedChangesSet();

		/**
		 * Initialize with single-member sets of changes (no links).
		 */
		LinkedChangesSet(const ChangeDependencyGraph& cdgA, const ChangeDependencyGraph& cdgB);

		/**
		 * This returns the linkedChanges which contains the change with
		 * \a changeId which is in branch A if and only if \a inBranchA is true.
		 * NOTE The \a inBranchA argument is necessary because IDs are not unique within LinkedChangesSet objects.
		 * Pointer comparison does not work because we made a deep copy.
		 */
		LinkedChanges findLinkedChanges(Model::NodeIdType oldChangeId, bool inBranchA);

		QSet<const std::shared_ptr<const ChangeDescription>> changesOfBranchA_;
};

/**
 * Creates and returns a new empty LinkedChanges object.
 */
inline LinkedChanges newLinkedChanges() { return std::make_shared<QSet<std::shared_ptr<const ChangeDescription>>>(); }

/**
 * Creates and returns a new LinkedChanges object that is a deep copy of \a changesToCopy.
 * Nodes pointed to by changes in the returned object are allocated in \a tree.
 */
LinkedChanges FILEPERSISTENCE_API copyLinkedChanges(const LinkedChanges& changesToCopy,
										  const QSet<const std::shared_ptr<const ChangeDescription>>& oldChangesOfA,
										  QSet<const std::shared_ptr<const ChangeDescription>>& newChangesOfA,
										  std::shared_ptr<GenericTree>& tree);

inline uint qHash(const LinkedChanges& linkedChanges, uint seed = 0)
{
	return ::qHash(linkedChanges.get(), seed);
}

}
