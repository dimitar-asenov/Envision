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
			  const GitRepository* repository)
{
	treeA_ = treeA;
	treeB_ = treeB;

	IdToGenericNodeHash nodesAHash;
	for (auto node : nodesA)
		nodesAHash.insertMulti(node->id(), node);

	IdToGenericNodeHash nodesBHash;
	for (auto node : nodesB)
		nodesBHash.insertMulti(node->id(), node);

	IdToGenericNodeHash newlyCreatedParents;
	findParentsInCommit(nodesAHash, newlyCreatedParents, treeA_, repository);
	findParentsInCommit(nodesBHash, newlyCreatedParents, treeB_, repository);

	filterPersistenceUnits(nodesAHash);
	filterPersistenceUnits(nodesBHash);

	idMatching(nodesAHash, nodesBHash, newlyCreatedParents);

	setAllChildStructureUpdates();
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
void Diff::idMatching(IdToGenericNodeHash& nodesA, IdToGenericNodeHash& nodesB,
							 IdToGenericNodeHash& createdParents)
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
			// found id
			changeDescriptions_.insert(nodeA->id(), new ChangeDescription(nodeA, iter.value()));
			// id is also present in nodesA
			onlyInNodesB.remove(iter.key());
		}
	}

	for (auto id : onlyInNodesB)
	{
		iter = nodesB.find(id);
		changeDescriptions_.insert(id, new ChangeDescription(nullptr, iter.value()));
	}

	for (auto parent : createdParents.values())
		changeDescriptions_.insert(parent->id(), new ChangeDescription(parent, parent));
}

void Diff::findParentsInCommit(IdToGenericNodeHash& nodes, IdToGenericNodeHash& createdParents,
										 std::shared_ptr<GenericTree> tree, const GitRepository* repository)
{
	QMultiHash<QString, Model::NodeIdType> fileToNodeIDs;
	for (auto node : nodes.values())
		fileToNodeIDs.insertMulti(node->persistentUnit()->name(), node->id());

	const QString fullFile("Diff::findParentsInCommit");

	IdToGenericNodeHash::iterator iter = nodes.end();
	for (auto path : fileToNodeIDs.uniqueKeys())
	{
		GenericPersistentUnit* unit = tree->persistentUnit(path);
		const CommitFile* file = repository->getCommitFile(tree->commitName(), path);
		GenericNode* root = Parser::load(file->content(), file->size_, false, tree->newPersistentUnit(fullFile));
		for (auto id : fileToNodeIDs.values(path))
		{
			GenericNode* nodeInFile = root->find(id);
			GenericNode* parentInFile = nodeInFile->parent();
			iter = nodes.find(id);
			Q_ASSERT(iter != nodes.end());
			GenericNode* node = iter.value();
			iter = nodes.find(parentInFile->id());
			GenericNode* parent = nullptr;
			if (iter == nodes.end())
			{
				parent = unit->newNode(parentInFile);
				createdParents.insert(parent->id(), parent);
			}
			else
				parent = iter.value();
			node->setParent(parent);
			parent->addChild(node);
		}
		tree->remove(fullFile);
	}
}

void Diff::setAllChildStructureUpdates()
{
	for (ChangeDescription* change : changeDescriptions_.values())
	{
		switch (change->type())
		{
			case ChangeType::Added:
				setChildStructureUpdateForNode(change->nodeB()->parent());
				break;

			case ChangeType::Deleted:
				setChildStructureUpdateForNode(change->nodeA()->parent());
				break;

			case ChangeType::Moved:
				setChildStructureUpdateForNode(change->nodeA()->parent());
				setChildStructureUpdateForNode(change->nodeB()->parent());
				break;

			case ChangeType::Stationary:
				if (change->flags().testFlag(ChangeDescription::Order))
					setChildStructureUpdateForNode(change->nodeA()->parent());
				break;

			default:
				Q_ASSERT(false);
		}
	}
}

void Diff::setChildStructureUpdateForNode(const GenericNode* node)
{
	if (node)
	{
		auto changeDescription = changeDescriptions_.value(node->id());
		Q_ASSERT(changeDescription);
		changeDescription->setChildrenUpdate(true);
	}
}

void Diff::filterPersistenceUnits(IdToGenericNodeHash& nodes)
{
	IdToGenericNodeHash::iterator i;
	GenericNode* previous = nullptr;
	for (i = nodes.begin(); i != nodes.end(); ++i)
	{
		if (previous && (previous->id() == i.value()->id()))
		{
			GenericNode* persistenceUnitDefinition = nullptr;
			GenericNode* persistenceUnitRoot = nullptr;

			if (previous->type() == GenericNode::persistentUnitType)
			{
				persistenceUnitDefinition = previous;
				persistenceUnitRoot = i.value();
			}
			else
			{
				Q_ASSERT(i.value()->type() == GenericNode::persistentUnitType);
				persistenceUnitDefinition = i.value();
				persistenceUnitRoot = previous;
			}

			// reset child / parent references correctly and remove PU definition
			persistenceUnitRoot->setParent(persistenceUnitDefinition->parent());
			persistenceUnitDefinition->remove();
			if (persistenceUnitRoot->parent())
				persistenceUnitRoot->parent()->addChild(persistenceUnitRoot);

			nodes.remove(previous->id());
			nodes.insert(previous->id(), persistenceUnitRoot);
		}
		previous = i.value();
	}
}

} /* namespace FilePersistence */
