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

#include "LinkedChangesSet.h"

#include "ChangeDependencyGraph.h"
#include "../simple/GenericTree.h"

namespace FilePersistence {

LinkedChangesSet::LinkedChangesSet() : QSet<LinkedChanges>{} {}

LinkedChangesSet::LinkedChangesSet(const ChangeDependencyGraph &cdgA, const ChangeDependencyGraph &cdgB)
{
	std::shared_ptr<GenericTree> tree = std::shared_ptr<GenericTree>{new GenericTree{"AllocatorForChanges"}};
	for (auto change : cdgA.changes().values())
	{
		auto linkedChanges = newLinkedChanges();
		auto copy = change->copy(tree, true);
		linkedChanges->insert(copy);
		this->insert(linkedChanges);
		changesOfBranchA_.insert(copy);
	}
	for (auto change : cdgB.changes().values())
	{
		auto linkedChanges = newLinkedChanges();
		linkedChanges->insert(change->copy(tree, true));
		this->insert(linkedChanges);
	}
}

LinkedChanges LinkedChangesSet::findLinkedChanges(Model::NodeIdType oldChangeId, bool inBranchA)
{
	for (auto linkedChanges : *this)
		for (auto change : *linkedChanges)
			if (change->nodeId() == oldChangeId && (changesOfBranchA_.contains(change) == inBranchA))
				return linkedChanges;
	Q_ASSERT(false);
}

LinkedChanges copyLinkedChanges(const LinkedChanges& changesToCopy,
										  const QSet<const std::shared_ptr<const ChangeDescription>>& oldChangesOfA,
										  QSet<const std::shared_ptr<const ChangeDescription>>& newChangesOfA,
										  std::shared_ptr<GenericTree>& tree)
{
	auto linkedChanges = newLinkedChanges();
	for (auto change : *changesToCopy)
	{
		auto newChange = change->copy(tree, true);
		linkedChanges->insert(newChange);
		if (oldChangesOfA.contains(change))
			newChangesOfA.insert(newChange);
	}
	return linkedChanges;
}

}
