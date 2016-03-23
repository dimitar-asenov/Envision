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

#include "Diff.h"

#include "../simple/GenericNode.h"

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

	computeChanges(nodesAHash, nodesBHash);
	computeStructChanges();
}

IdToChangeDescriptionHash Diff::changes(ChangeType type) const
{
	IdToChangeDescriptionHash changesOfType;
	for (auto change : changeDescriptions_.values())
		if (change->type() == type)
			changesOfType.insert(change->nodeId(), change);
	return changesOfType;
}

IdToChangeDescriptionHash Diff::changes(ChangeType type, ChangeDescription::UpdateFlags flags) const
{
	IdToChangeDescriptionHash changesOfType;
	for (auto change : changeDescriptions_.values())
		if (change->type() == type && change->hasFlags(flags))
			changesOfType.insert(change->nodeId(), change);
	return changesOfType;
}

void Diff::computeChanges(IdToGenericNodeHash& nodesA, IdToGenericNodeHash& nodesB)
{
	QSet<Model::NodeIdType> onlyInNodesB = QSet<Model::NodeIdType>::fromList(nodesB.keys());
	IdToGenericNodeHash::iterator iter;
	for (auto nodeA : nodesA.values())
	{
		iter = nodesB.find(nodeA->id());
		if (iter == nodesB.end())
		{
			// no such id in nodesB
			auto change = std::make_shared<ChangeDescription>(nodeA, nullptr);
			changeDescriptions_.insert(nodeA->id(), change);
		}
		else
		{
			// found id in nodesB
			auto change = std::make_shared<ChangeDescription>(nodeA, iter.value());
			changeDescriptions_.insert(nodeA->id(), change);
			onlyInNodesB.remove(iter.key());
		}
	}
	// Intermediate state 2
	// See report for details.
	// TODO add link to report
	for (auto id : onlyInNodesB)
	{
		iter = nodesB.find(id);
		auto change = std::make_shared<ChangeDescription>(nullptr, iter.value());
		changeDescriptions_.insert(id, change);
	}
	for (auto change : changeDescriptions_.values())
		if (change->isFake())
			changeDescriptions_.remove(change->nodeId());
}

void Diff::computeStructChanges()
{
	for (auto change : changeDescriptions_.values())
	{
		if (change->type() == ChangeType::Insertion || change->type() == ChangeType::Move)
			setStructureFlagForId(change->nodeB()->parentId(), change);
		if (change->type() == ChangeType::Deletion || change->type() == ChangeType::Move
			 || change->hasFlags(ChangeDescription::Label))
			setStructureFlagForId(change->nodeA()->parentId(), change);
	}
}

void Diff::setStructureFlagForId(Model::NodeIdType id, std::shared_ptr<ChangeDescription> causingChange)
{
	if (id.isNull()) return;
	if (!changeDescriptions_.contains(id))
	{
		auto change = ChangeDescription::newStructChange(id, causingChange, treeA_, treeB_);
		changeDescriptions_.insert(id, change);
		change->setStructureChangeFlag(true);
	}
	else
		changeDescriptions_.value(id).get()->setStructureChangeFlag(true);

	Q_ASSERT(changeDescriptions_.find(id).value()->hasFlags(ChangeDescription::Structure));
	Q_ASSERT(changeDescriptions_.value(id)->debugHasNodes());
}

void Diff::filterPersistenceUnits(IdToGenericNodeHash& nodes)
{
	for (auto node : nodes.values())
		if (node->type() == GenericNode::PERSISTENT_UNIT_TYPE)
			nodes.remove(node->id(), node);
}

}
