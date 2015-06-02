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


#include "ChangeDependencyGraph.h"

namespace FilePersistence {

ChangeDependencyGraph::ChangeDependencyGraph(Diff& diff) : changes_{diff.changes()}
{
	for (auto change : changes_.values())
	{
		if (change->type() == ChangeType::Insertion || change->type() == ChangeType::Move)
		{
			auto it = changes_.find(change->nodeB()->parentId());
			while (it != changes_.end() && it.key() == change->nodeB()->parentId())
			{
				if (it.value()->type() == ChangeType::Insertion) addDependency(change, it.value());
				it++;
			}
		}
		if (change->type() == ChangeType::Deletion || change->type() == ChangeType::Move)
		{
			auto it = changes_.find(change->nodeA()->parentId());
			while (it != changes_.end() && it.key() == change->nodeA()->parentId())
			{
				if (it.value()->type() == ChangeType::Deletion) addDependency(change, it.value());
				it++;
			}
		}
	}
}

ChangeDependencyGraph::~ChangeDependencyGraph() {}

void ChangeDependencyGraph::insert(std::shared_ptr<const ChangeDescription>& change)
{
	Q_ASSERT(!changes_.contains(change->id()));
	changes_.insert(change->id(), change);
}

void ChangeDependencyGraph::remove(std::shared_ptr<const ChangeDescription>& change)
{
	Q_ASSERT(changes_.value(change->id()) == change);
	changes_.remove(change->id());
	for (auto dependingOnChange : dependencies_.keys(change))
	{
		dependencies_.remove(dependingOnChange, change);
	}
	dependencies_.remove(change);
}

void ChangeDependencyGraph::replace(std::shared_ptr<const ChangeDescription>& oldChange,
				 std::shared_ptr<const ChangeDescription>& newChange)
{
	Q_ASSERT(changes_.value(oldChange->id()) == oldChange);
	auto dependsOn = dependencies_.values(oldChange);
	auto dependingOnChange = dependencies_.keys(oldChange);
	for (auto change : dependingOnChange)
	{
		dependencies_.remove(change, oldChange);
		dependencies_.insert(change, newChange);
	}
	dependencies_.remove(oldChange);
	for (auto change : dependsOn)
		dependencies_.insert(newChange, change);
}

void ChangeDependencyGraph::addDependency(std::shared_ptr<const ChangeDescription>& changeA,
														std::shared_ptr<const ChangeDescription>& changeB)
{
	Q_ASSERT(changes_.values().contains(changeA));
	Q_ASSERT(changes_.values().contains(changeB));
	dependencies_.insert(changeA, changeB);
}

void ChangeDependencyGraph::removeDependency(std::shared_ptr<const ChangeDescription>& changeA,
															std::shared_ptr<const ChangeDescription>& changeB)
{
	dependencies_.remove(changeA, changeB);
}

} /* namespace FilePersistence */
