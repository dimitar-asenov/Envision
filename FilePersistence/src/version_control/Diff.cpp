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

#include <QDebug>

#include <iostream>

namespace FilePersistence {

Diff::Diff(IdToGenericNodeHash oldNodes, IdToGenericNodeHash newNodes)
{
	Q_ASSERT(changeDescriptions_.isEmpty());
	idMatching(oldNodes, newNodes);
	includeAndMarkParents();
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

void Diff::includeAndMarkParents()
{
	QList<ChangeDescription*> beforeIncludingParents = changeDescriptions_.values();

	for (ChangeDescription* description : beforeIncludingParents)
	{
		ChangeDescription::UpdateFlags flags = description->flags();
		switch (description->type())
		{
			case ChangeType::Added:
				includeAndMarkParent(description->newNode());
				break;

			case ChangeType::Deleted:
				includeAndMarkParent(description->oldNode());
				break;

			case ChangeType::Moved:
				includeAndMarkParent(description->oldNode());
				includeAndMarkParent(description->newNode());
				break;

			case ChangeType::Stationary:
				// need to be a reordering
				if (flags.testFlag(ChangeDescription::Order))
				{
					Q_ASSERT(description->oldNode()->parent()->id() == description->newNode()->parent()->id());
					includeAndMarkParent(description->oldNode());
				}
				break;

			default:
				Q_ASSERT(false);
		}
	}
}

void Diff::includeAndMarkParent(const GenericNode* child)
{
	if (child->parent() != nullptr)
	{
		ChangeDescription* description = nullptr;
		IdToChangeDescriptionHash::iterator iter = changeDescriptions_.find(child->parent()->id());
		if (iter == changeDescriptions_.end())
		{
			// no parent in changeDescriptions
			description = new ChangeDescription(child->parent(), child->parent());
			description->setChildrenUpdate(true);
			changeDescriptions_.insert(child->parent()->id(), description);
		}
		else
		{
			// parent already exists in changeDescriptions
			description = iter.value();
			description->setChildrenUpdate(true);
		}
	}
}

} /* namespace FilePersistence */
