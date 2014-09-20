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

#include "ChangeDescription.h"

namespace FilePersistence {

ChangeDescription::ChangeDescription(GenericNode* oldNode, GenericNode* newNode)
{
	Q_ASSERT(oldNode != nullptr || newNode != nullptr);

	oldNode_ = oldNode;
	newNode_ = newNode;

	fundamentalChangeClassification();
	if (type_ == ChangeType::Moved || type_ == ChangeType::Stationary)
	{
		detectReorder();
		detectTypeUpdate();
		detectValueUpdate();
	}
}

void ChangeDescription::fundamentalChangeClassification()
{
	if (oldNode_ == nullptr)
		type_ = ChangeType::Added;
	else
	{
		if (newNode_ == nullptr)
			type_ = ChangeType::Deleted;
		else
		{
			if (oldNode_->parent() == nullptr || newNode_->parent() == nullptr)
			{
				if (oldNode_->parent() == nullptr && newNode_->parent() == nullptr)
					type_ = ChangeType::Stationary;
				else
					type_ = ChangeType::Moved;
			}
			else
			{
				if (oldNode_->parent()->id() == newNode_->parent()->id())
					type_ = ChangeType::Stationary;
				else
					type_ = ChangeType::Moved;
			}
		}
	}
}

void ChangeDescription::detectReorder()
{
	// check for same name -> reordering detection
	int reorder = QString::compare(oldNode_->name(), newNode_->name());
	if (reorder != 0)
		updateFlags_ |= Order;
	else
		updateFlags_ &= ~Order;
}

void ChangeDescription::detectValueUpdate()
{


	// check for same type -> type change
	int typeChange = QString::compare(oldNode_->rawValue(), newNode_->rawValue());
	if (typeChange != 0)
		updateFlags_ |= Value;
	else
		updateFlags_ &= ~Value;
}

void ChangeDescription::detectTypeUpdate()
{
	// check for same value -> update
	int valueUpdate = QString::compare(oldNode_->type(), newNode_->type());
	if (valueUpdate != 0)
		updateFlags_ |= Type;
	else
		updateFlags_ &= ~Type;
}

void ChangeDescription::print() const
{
	std::cout << id().toString().toStdString().c_str() << "\t";
	switch (type_)
	{
		case ChangeType::Added:
			std::cout << "Added" << std::endl;
			break;

		case ChangeType::Deleted:
			std::cout << "Deleted" << std::endl;
			break;

		case ChangeType::Moved:
			std::cout << "Moved" << std::endl;
			break;

		case ChangeType::Stationary:
			std::cout << "Stationary" << std::endl;
			break;

		case ChangeType::Unclassified:
			std::cout << "Unclassified" << std::endl;
			break;

		default:
			Q_ASSERT(false);
	}

	if (updateFlags_.testFlag(Order))
		std::cout << " Location";
	if (updateFlags_.testFlag(Type))
		std::cout << " Type";
	if (updateFlags_.testFlag(Value))
		std::cout << " Value";
	if (updateFlags_.testFlag(Children))
		std::cout << " Children";
	std::cout << std::endl;

}

void ChangeDescription::setChildrenUpdate(bool isUpdate)
{
	if (type_ == ChangeType::Moved || type_ == ChangeType::Stationary)
	{
		if (isUpdate)
			updateFlags_ |= Children;
		else
			updateFlags_ &= ~Children;
	}
}

Model::NodeIdType ChangeDescription::id() const
{
	if (type_ == ChangeType::Added)
		return newNode_->id();
	else
		return oldNode_->id();
}

} /* namespace FilePersistence */
