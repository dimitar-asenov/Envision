/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

ChangeDependencyGraph::ChangeDependencyGraph(Diff& diff)
{
	changes_ = {};
	map_ = {};
	foreach (ChangeDescription* change, diff.changes().values())
	{
		changes_.insert(change);
		if (change->type() == ChangeType::Added || change->type() == ChangeType::Moved)
		{
			QHash<Model::NodeIdType, ChangeDescription*>::iterator it = diff.changes().find(change->nodeB()->parentId());
			while (it != diff.changes().end() && it.key() == change->nodeB()->parentId())
			{
				if (it.value()->type() == ChangeType::Added) map_.insert(change, it.value());
				it++;
			}
		}
		if (change->type() == ChangeType::Deleted || change->type() == ChangeType::Moved)
		{
			QHash<Model::NodeIdType, ChangeDescription*>::iterator it = diff.changes().find(change->nodeA()->parentId());
			while (it != diff.changes().end() && it.key() == change->nodeA()->parentId())
			{
				if (it.value()->type() == ChangeType::Deleted) map_.insert(it.value(), change);
				it++;
			}
		}
	}
}

ChangeDependencyGraph::~ChangeDependencyGraph() {}

void ChangeDependencyGraph::addDependency(ChangeDescription* changeA, ChangeDescription* changeB)
{
	Q_ASSERT(changes_.contains(changeA));
	Q_ASSERT(changes_.contains(changeB));
	map_.insert(changeA, changeB);
}

void ChangeDependencyGraph::removeDependecy(ChangeDescription* changeA, ChangeDescription* changeB)
{
	Q_ASSERT(map_.contains(changeA, changeB));
	map_.remove(changeA, changeB);
}

} /* namespace FilePersistence */
