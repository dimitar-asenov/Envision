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

Diff::Diff(QList<GenericNode*> oldNodes, GenericTree* oldTree,
	  QList<GenericNode*> newNodes, GenericTree* newTree,
	  const GitRepository* repository)
{
	oldTree_ = oldTree;
	newTree_ = newTree;

	IdToGenericNodeHash oldNodesHash;
	for (GenericNode* node : oldNodes)
		oldNodesHash.insert(node->id(), node);

	IdToGenericNodeHash newNodesHash;
	for (GenericNode* node : newNodes)
		newNodesHash.insert(node->id(), node);

	findParentsInCommit(oldNodesHash, oldTree_, repository);
	findParentsInCommit(newNodesHash, newTree_, repository);

	idMatching(oldNodesHash, newNodesHash);
}

Diff::~Diff()
{
	delete oldTree_;
	delete newTree_;
}

void Diff::print() const
{
	QList<ChangeDescription*> descriptions = changeDescriptions_.values();

	for (ChangeDescription* current : descriptions)
	{
		std::cout << "----------------------------------------" << std::endl;
		current->print();
	}
	std::cout << "----------------------------------------" << std::endl;
}

IdToChangeDescriptionHash Diff::changes(ChangeType type) const
{
	IdToChangeDescriptionHash changesOfType;
	for (ChangeDescription* change : changeDescriptions_.values())
	{
		if (change->type() == type)
			changesOfType.insert(change->id(), change);
	}
	return changesOfType;
}

IdToChangeDescriptionHash Diff::changes(ChangeType type, ChangeDescription::UpdateFlags flags) const
{
	IdToChangeDescriptionHash changesOfType;
	for (ChangeDescription* change : changeDescriptions_.values())
	{
		if (change->type() == type &&
			 ChangeDescription::compareUpdateFlags(flags, change->flags()))
			changesOfType.insert(change->id(), change);
	}
	return changesOfType;
}

// Private methods
void Diff::idMatching(IdToGenericNodeHash oldNodes, IdToGenericNodeHash newNodes)
{
	QSet<Model::NodeIdType> onlyInNewNodes = QSet<Model::NodeIdType>::fromList(newNodes.keys());
	QList<GenericNode*> oldNodesValueList = oldNodes.values();

	ChangeDescription* description = nullptr;

	IdToGenericNodeHash::iterator iter;
	for (GenericNode* oldNode : oldNodesValueList)
	{
		iter = newNodes.find(oldNode->id());
		if (iter == newNodes.end())
		{
			// no such id in newNodes
			description = new ChangeDescription(oldNode, nullptr);
			changeDescriptions_.insert(oldNode->id(), description);
		}
		else
		{
			// found id
			description = new ChangeDescription(oldNode, iter.value());
			changeDescriptions_.insert(oldNode->id(), description);
			// id is also present in oldNodes
			onlyInNewNodes.remove(iter.key());
		}
	}

	for (Model::NodeIdType newId : onlyInNewNodes)
	{
		iter = newNodes.find(newId);
		description = new ChangeDescription(nullptr, iter.value());
		changeDescriptions_.insert(newId, description);
	}
}

void Diff::findParentsInCommit(IdToGenericNodeHash nodes, GenericTree* tree,
										 const GitRepository* repository)
{
	QHash<QString, Model::NodeIdType> fileToNodeIDs;
	for (GenericNode* node : nodes.values())
		fileToNodeIDs.insert(node->persistentUnit()->name(), node->id());

	const QString fullFile("Diff::findParentsInCommit");

	IdToGenericNodeHash::iterator iter = nodes.end();
	for (QString path : fileToNodeIDs.uniqueKeys())
	{
		GenericPersistentUnit fullFileUnit = tree->newPersistentUnit(fullFile);
		GenericPersistentUnit* unit = tree->persistentUnit(path);
		CommitFile file = repository->getCommitFile(tree->commitName(), path);
		GenericNode* root = Parser::load(file.content_, file.size_, false, fullFileUnit);
		for (Model::NodeIdType id : fileToNodeIDs.values(path))
		{
			GenericNode* nodeInFile = root->find(id);
			GenericNode* parentInFile = nodeInFile->parent();
			Q_ASSERT(nodes.contains(id));
			iter = nodes.find(id);
			GenericNode* node = iter.value();
			iter = nodes.find(parentInFile->id());
			GenericNode* parent = nullptr;
			if (iter == nodes.end())
			{
				parent = unit->newNode(parentInFile);
				nodes.insert(parent->id(), parent);
			}
			else
				parent = iter.value();
			node->setParent(parent);
			parent->addChild(node);
		}
		tree->remove(fullFileUnit);
	}
}

} /* namespace FilePersistence */
