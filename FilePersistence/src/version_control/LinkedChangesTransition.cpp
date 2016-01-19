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

#include "LinkedChangesTransition.h"

namespace FilePersistence {

LinkedChangesTransition::LinkedChangesTransition() {}

LinkedChangesTransition::LinkedChangesTransition(const LinkedChangesSet& linkedChangesSet) :
	oldLinkedChangesSet_{linkedChangesSet} {}

LinkedChangesTransition::LinkedChangesTransition(const LinkedChangesSet& linkedChangesSet,
																 const ChangeDependencyGraph& cdgA,
																 const ChangeDependencyGraph& cdgB) :
	oldLinkedChangesSet_{linkedChangesSet}
{
	for (auto linkedChanges : linkedChangesSet)
	{
		auto nLinkedChanges = newLinkedChanges();
		for (auto oldChange : *linkedChanges)
		{
			bool inA = linkedChangesSet.changesOfBranchA_.contains(oldChange);
			std::shared_ptr<const ChangeDescription> newChange = inA ? cdgA.changes().value(oldChange->nodeId())
																						: cdgB.changes().value(oldChange->nodeId());
			nLinkedChanges->insert(newChange);
			if (inA) changesOfBranchA_.insert(newChange);
		}
		transition_.insert(linkedChanges, nLinkedChanges);
	}
}

void LinkedChangesTransition::insert(Model::NodeIdType oldChangeId, bool oldInBranchA,
												 const std::shared_ptr<const ChangeDescription>& change,
												 bool newInBranchA)
{
	auto keySet = oldLinkedChangesSet_.findLinkedChanges(oldChangeId, oldInBranchA);
	// NOTE could probably be optimized
	if (transition_.contains(keySet) && transition_.value(keySet)->contains(change)) return; // already mapped

	LinkedChanges setContainingChange;
	bool changeIsMappedTo = false;
	for (auto linkedChanges : transition_.values())
	{
		if (linkedChanges->contains(change))
		{
			setContainingChange = linkedChanges;
			changeIsMappedTo = true;
			break;
		}
	}

	if (changeIsMappedTo)
	{
		auto currentlyMappedSet = transition_.value(keySet);
		setContainingChange->unite(*currentlyMappedSet);
		transition_.insert(keySet, setContainingChange);
	}
	else
	{
		if (transition_.contains(keySet)) transition_.value(keySet)->insert(change);
		else
		{
			auto newSet = newLinkedChanges();
			newSet->insert(change);
			transition_.insert(keySet, newSet);
		}
		if (newInBranchA) changesOfBranchA_.insert(change);
	}
}

const LinkedChangesSet LinkedChangesTransition::getNewState() const
{
	std::shared_ptr<GenericTree> tree(new GenericTree{"AllocatorForChanges"});
	LinkedChangesSet set;
	for (auto linkedChanges : transition_)
	{
		LinkedChanges newLinkedChanges = copyLinkedChanges(linkedChanges, changesOfBranchA_, set.changesOfBranchA_, tree);
		set.insert(newLinkedChanges);
	}
	return set;
}

}
