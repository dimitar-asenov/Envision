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
#include "ConflictPairs.h"

namespace FilePersistence {

using RelationSet = std::shared_ptr<QSet<std::shared_ptr<const ChangeDescription>>>;
using RelationAssignment = QSet<RelationSet>;
using RelationAssignmentTransition = QHash<RelationSet, RelationSet>;

class ConflictPipelineComponent
{
	public:
		virtual ~ConflictPipelineComponent();
		virtual RelationAssignmentTransition run(const std::unique_ptr<GenericTree>& treeBase,
															  const std::unique_ptr<GenericTree>& treeA,
															  const std::unique_ptr<GenericTree>& treeB,
															  ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
															  QSet<std::shared_ptr<const ChangeDescription>>& conflictingChanges,
															  ConflictPairs& conflictPairs, RelationAssignment& relationAssignment) = 0;
	protected:
		RelationAssignmentTransition createIdentityTransition(RelationAssignment& relationAssignment);
		RelationSet findRelationSet(std::shared_ptr<const ChangeDescription> change, RelationAssignment& relationAssignment);
};

inline uint qHash(const RelationSet& relationSet, uint seed = 0)
{
	return ::qHash(relationSet.get(), seed);
}

} /* namespace FilePersistence */
