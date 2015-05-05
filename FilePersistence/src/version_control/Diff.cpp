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

	computeChanges(nodesAHash, nodesBHash);
	computeStructChanges();
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
			changeDescriptions_.insert(nodeA->id(), new ChangeDescription(nodeA, nullptr));
		}
		else
		{
			// found id in nodesB
			changeDescriptions_.insert(nodeA->id(), new ChangeDescription(nodeA, iter.value()));
			onlyInNodesB.remove(iter.key());
		}
	}
	// Intermediate state 2
	for (auto id : onlyInNodesB)
	{
		iter = nodesB.find(id);
		changeDescriptions_.insert(id, new ChangeDescription(nullptr, iter.value()));
	}
	// Intermediate state 3
	for (ChangeDescription* change : changeDescriptions_)
	if (!isModifying(change)) changeDescriptions_.remove(change->id());
}

/**
 * Sets \a structFlag for appropriate changes and may create new changes.
 */
void Diff::computeStructChanges()
{
	for (ChangeDescription* change : changeDescriptions_.values())
	{
		if (change->type() == ChangeType::Added || change->type() == ChangeType::Moved)
			setStructFlagForId(change->nodeB()->parentId());
		if (change->type() == ChangeType::Deleted || change->type() == ChangeType::Moved)
			setStructFlagForId(change->nodeA()->parentId());
		if (change->hasFlags(ChangeDescription::Label))
			setStructFlagForId(change->nodeA()->parentId());
	}
}

/**
 * If a change for \a id already exists, its \a structFlag is set,
 * otherwise a new change with that flag is created.
 */
void Diff::setStructFlagForId(const Model::NodeIdType id)
{
	IdToChangeDescriptionHash::iterator changeIt = changeDescriptions_.find(id);
	ChangeDescription* change;
	if (changeIt == changeDescriptions_.end())
	{
		change = new ChangeDescription(ChangeType::Stationary);
		changeDescriptions_.insert(id, change);
		// TODO Problem? These changes have no node, just an id.
	}
	else
		change = changeIt.value();
	change->setChildrenUpdate(true);

	Q_ASSERT(changeDescriptions_.find(id).value()->hasFlags(ChangeDescription::Children));
}

/**
 * removes the nodes that link PersistentUnits from \a nodes.
 */
void Diff::filterPersistenceUnits(IdToGenericNodeHash&)
{
	// TODO: implement
}

} /* namespace FilePersistence */
