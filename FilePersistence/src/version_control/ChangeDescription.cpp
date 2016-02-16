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

#include "ChangeDescription.h"

namespace FilePersistence {

ChangeDescription::ChangeDescription(GenericNode* nodeA, GenericNode* nodeB) :
	pointsToChildA_{false}, pointsToChildB_{false}, nodeA_{nodeA}, nodeB_{nodeB}
{
	Q_ASSERT(nodeA_ || nodeB_);
	if (nodeA_ && nodeB_) Q_ASSERT(nodeA_->id() == nodeB->id());

	if (nodeA_)
	{
		nodeId_ = nodeA_->id();
		if (nodeB_)
		{
			if (nodeA_->parentId() == nodeB_->parentId()) type_ = ChangeType::Stationary;
			else type_ = ChangeType::Move;
		}
		else type_ = ChangeType::Deletion;
	}
	else
	{
		nodeId_ = nodeB_->id();
		type_ = ChangeType::Insertion;
	}
	computeFlags();
}

QString ChangeDescription::summary() const
{
	QString typeStr;
	switch (type_)
	{
		case ChangeType::Deletion:
			typeStr = "Deletion";
			break;
		case ChangeType::Insertion:
			typeStr = "Insertion";
			break;
		case ChangeType::Move:
			typeStr = "Move";
			break;
		case ChangeType::Stationary:
			typeStr = "Stationary";
			break;
		default:
			Q_ASSERT(false);
	}

	QString flagStr;
	if (updateFlags_.testFlag(Label))
		flagStr += "Label";
	if (updateFlags_.testFlag(Value))
	{
		if (!flagStr.isEmpty())
			flagStr += ", ";
		flagStr += "Value";
	}
	if (updateFlags_.testFlag(Type))
	{
		if (!flagStr.isEmpty())
			flagStr += ", ";
		flagStr += "Type";
	}
	if (updateFlags_.testFlag(Structure))
	{
		if (!flagStr.isEmpty())
			flagStr += ", ";
		flagStr += "Struct";
	}

	auto summary = "{..." + nodeId_.toString().right(7) + ":" + typeStr;
	if (updateFlags_ != NoFlags)
		summary += " (" + flagStr + ")";
	return summary;
}

std::shared_ptr<ChangeDescription> ChangeDescription::newStructChange(
		Model::NodeIdType nodeId,
		std::shared_ptr<ChangeDescription> causingChange,
		std::shared_ptr<GenericTree> treeA,
		std::shared_ptr<GenericTree> treeB)
{
	Q_ASSERT(causingChange->debugHasNodes());
	auto childA = causingChange->nodeA();
	auto childB = causingChange->nodeB();
	std::shared_ptr<ChangeDescription> change{new ChangeDescription};
	change->nodeId_ = nodeId;
	change->type_ = ChangeType::Stationary;
	change->updateFlags_ = UpdateType::Structure;

	if (causingChange->type() == ChangeType::Stationary)
	{
		change->nodeA_ = childA;
		change->nodeB_ = childB;
		change->pointsToChildA_ = true;
		change->pointsToChildB_ = true;
	}
	else if (causingChange->type() == ChangeType::Deletion ||
				(causingChange->type() == ChangeType::Move && childA->parentId() == nodeId))
	{
		change->nodeA_ = childA;
		change->pointsToChildA_ = true;
		change->nodeB_ = treeB->find(nodeId, true);
		change->pointsToChildB_ = false;
	}
	else if (causingChange->type() == ChangeType::Insertion ||
			  (causingChange->type() == ChangeType::Move && childB->parentId() == nodeId))
	{
		change->nodeA_ = treeA->find(nodeId, true);
		change->pointsToChildA_ = false;
		change->nodeB_ = childB;
		change->pointsToChildB_ = true;
	}
	else
		Q_ASSERT(false);

	return change;
}

void ChangeDescription::computeFlags()
{
	if (nodeA_ != nullptr && nodeB_ != nullptr)
	{
		if (nodeA_->label() != nodeB_->label()) updateFlags_ |= Label;
		else updateFlags_ &= ~Label;

		if (nodeA_->rawValue() != nodeB_->rawValue()) updateFlags_ |= Value;
		else updateFlags_ &= ~Value;

		if (nodeA_->type() != nodeB_->type()) updateFlags_ |= Type;
		else updateFlags_ &= ~Type;
	}
}

void ChangeDescription::print() const
{
	if (nodeA_ || nodeB_)
		qDebug() << (nodeA_ ? nodeA_->type() : nodeB_->type()) << "\t";
	qDebug() << nodeId().toString() << "\t";
	switch (type_)
	{
		case ChangeType::Insertion:
			qDebug() << "Insertion" << endl;
			break;

		case ChangeType::Deletion:
			qDebug() << "Deletion" << endl;
			break;

		case ChangeType::Move:
			qDebug() << "Move" << endl;
			break;

		case ChangeType::Stationary:
			qDebug() << "Stationary" << endl;
			break;

		case ChangeType::Unclassified:
			qDebug() << "Unclassified" << endl;
			break;

		default:
			Q_ASSERT(false);
	}

	if (updateFlags_.testFlag(Label)) qDebug() << "\tLabel";
	if (updateFlags_.testFlag(Type)) qDebug() << "\tType";
	if (updateFlags_.testFlag(Value)) qDebug() << "\tValue";
	if (updateFlags_.testFlag(Structure)) qDebug() << "\tStructure";
	qDebug() << endl;
}

void ChangeDescription::setStructureChangeFlag(bool value)
{
	if (value) updateFlags_ |= Structure;
	else updateFlags_ &= ~Structure;
}

GenericNode* ChangeDescription::nodeA() const
{
	if (pointsToChildA_)
	{
		const_cast<ChangeDescription*>(this)->nodeA_ = nodeA_->parent();
		const_cast<ChangeDescription*>(this)->pointsToChildA_ = false;
	}
	return nodeA_;
}

GenericNode* ChangeDescription::nodeB() const
{
	if (pointsToChildB_)
	{
		const_cast<ChangeDescription*>(this)->nodeB_ = nodeB_->parent();
		const_cast<ChangeDescription*>(this)->pointsToChildB_ = false;
	}
	return nodeB_;
}

std::shared_ptr<ChangeDescription> ChangeDescription::copy(std::shared_ptr<GenericTree>& tree, bool force) const
{
	// This method lazy-loads nodeA and nodeB. This is necessary because deep-copy trees can't lazy-load.
	GenericNode* newNodeA = nullptr;
	if (nodeA())
	{
		// NOTE This might be unnecessary because we never change nodeA, so why create a new one?
		auto persistentUnit = tree->persistentUnit(nodeA()->persistentUnit()->name());
		if (persistentUnit == nullptr)
			persistentUnit = &tree->newPersistentUnit(nodeA()->persistentUnit()->name());
		newNodeA = persistentUnit->newNode(nodeA(), force);
	}

	GenericNode* newNodeB = nullptr;
	if (nodeB())
	{
		auto persistentUnit = tree->persistentUnit(nodeB()->persistentUnit()->name());
		if (persistentUnit == nullptr)
			persistentUnit = &tree->newPersistentUnit(nodeB()->persistentUnit()->name());
		newNodeA = persistentUnit->newNode(nodeB(), force);
	}

	std::shared_ptr<ChangeDescription> newChange{new ChangeDescription};
	newChange->nodeId_ = nodeId_;
	newChange->nodeA_ = newNodeA;
	newChange->nodeB_ = newNodeB;
	newChange->pointsToChildA_ = false;
	newChange->pointsToChildB_ = false;
	newChange->type_ = type_;
	newChange->updateFlags_ = updateFlags_;
	return newChange;
}

}
