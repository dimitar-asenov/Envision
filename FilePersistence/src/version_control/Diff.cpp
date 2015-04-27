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

#include "Diff.h"

#include "GitRepository.h"

#include "../simple/Parser.h"

namespace FilePersistence {

Diff::Diff(){}

Diff::Diff(QList<GenericNode*>& nodesA, std::shared_ptr<GenericTree> treeA,
			  QList<GenericNode*>& nodesB, std::shared_ptr<GenericTree> treeB,
			  const GitRepository*)
{
	treeA_ = treeA;
	treeB_ = treeB;

	IdToGenericNodeHash nodesAHash;
	for (auto node : nodesA)
		nodesAHash.insertMulti(node->id(), node);

	IdToGenericNodeHash nodesBHash;
	for (auto node : nodesB)
		nodesBHash.insertMulti(node->id(), node);

	filterPersistenceUnits(nodesAHash);
	filterPersistenceUnits(nodesBHash);

	idMatching(nodesAHash, nodesBHash);

	// setAllChildStructureUpdates();
}

IdToChangeDescriptionHash Diff::changes(ChangeType type) const
{
	IdToChangeDescriptionHash changesOfType;
	for (auto change : changeDescriptions_.values())
	{
		if (change->type() == type)
			changesOfType.insert(change->id(), change);
	}
	return changesOfType;
}

IdToChangeDescriptionHash Diff::changes(ChangeType type, ChangeDescription::UpdateFlags flags) const
{
	IdToChangeDescriptionHash changesOfType;
	for (auto change : changeDescriptions_.values())
	{
		if (change->type() == type && change->hasFlags(flags))
			changesOfType.insert(change->id(), change);
	}
	return changesOfType;
}

// Private methods
void Diff::idMatching(IdToGenericNodeHash& nodesA, IdToGenericNodeHash& nodesB)
{
	QSet<Model::NodeIdType> onlyInNodesB = QSet<Model::NodeIdType>::fromList(nodesB.keys());
	QSet<Model::NodeIdType> changedParents{};

	IdToGenericNodeHash::iterator iter;
	for (auto nodeA : nodesA.values())
	{
		iter = nodesB.find(nodeA->id());
		if (iter == nodesB.end())
		{
			// no such id in nodesB
			changeDescriptions_.insert(nodeA->id(), new ChangeDescription(nodeA, nullptr));
		}
		else
		{
			// found id
			ChangeDescription* change = new ChangeDescription(nodeA, iter.value());
			if (Diff::trueChange(change))
			{
				changeDescriptions_.insert(nodeA->id(), change);
				if (change->type() == ChangeType::Moved)
				{
					changedParents.insert(nodeA->parentId());
					changedParents.insert(iter.value()->parentId());
				}
				else if (change->type() == ChangeType::Deleted || change->hasFlags(ChangeDescription::UpdateType::Label))
					changedParents.insert(nodeA->parentId());
			}
			// id is also present in nodesA
			onlyInNodesB.remove(iter.key());
		}
	}

	for (auto id : onlyInNodesB)
	{
		iter = nodesB.find(id);
		changeDescriptions_.insert(id, new ChangeDescription(nullptr, iter.value()));
		changedParents.insert(iter.value()->parentId());
	}

	for (auto parentId : changedParents.values())
	{
		ChangeDescription* change;
		auto iter = changeDescriptions_.find(parentId);
		if (iter == changeDescriptions_.end())
		{
			change = new ChangeDescription(ChangeType::Stationary);
			changeDescriptions_.insert(parentId, change);
		}
		else
			change = iter.value();
		change->setChildrenUpdate(true);
	}
}

/**
 * This method is only needed for one variant of idMatching
 */
void Diff::setAllChildStructureUpdates()
{
	for (ChangeDescription* change : changeDescriptions_.values())
	{
		if (change->type() == ChangeType::Added ||
			 change->type() == ChangeType::Deleted ||
			 change->type() == ChangeType::Moved ||
			 change->hasFlags(ChangeDescription::UpdateType::Label))
		{
			Model::NodeIdType parentId;
			if (change->nodeA())
				parentId = change->nodeA()->parentId();
			else
				parentId = change->nodeB()->parentId();

			ChangeDescription* parentChange;
			auto iter = changeDescriptions_.find(parentId);
			if (iter == changeDescriptions_.end())
			{
				parentChange = new ChangeDescription(ChangeType::Stationary);
				changeDescriptions_.insert(parentId, parentChange);
			}
			else
				parentChange = iter.value();
			parentChange->setChildrenUpdate(true);
		}
	}
}

/**
 * removes the nodes that link PersistentUnits from \a nodes.
 */
void Diff::filterPersistenceUnits(IdToGenericNodeHash&)
{
	// TODO: implement
}

} /* namespace FilePersistence */
