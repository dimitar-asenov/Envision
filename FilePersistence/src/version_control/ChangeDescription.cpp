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

ChangeDescription::ChangeDescription(GenericNode* nodeA, GenericNode* nodeB) : nodeA_{nodeA}, nodeB_{nodeB}
{
	Q_ASSERT(nodeA_ != nullptr || nodeB_ != nullptr);

	if (nodeA_)
	{
		id_ = nodeA_->id();
		if (nodeB_)
		{
			if (nodeA_->parentId() == nodeB_->parentId())
				type_ = ChangeType::Stationary;
			else
				type_ = ChangeType::Move;
		}
		else
			type_ = ChangeType::Deletion;
	}
	else
	{
		id_ = nodeB_->id();
		type_ = ChangeType::Insertion;
	}
	setFlags();
}

ChangeDescription::ChangeDescription(Model::NodeIdType id, ChangeType type) : id_{id}, type_{type} {}

void ChangeDescription::setFlags()
{
	if (nodeA_ != nullptr && nodeB_ != nullptr)
	{
		if (nodeA_->name() != nodeB_->name())
			updateFlags_ |= Label;
		else
			updateFlags_ &= ~Label;

		if (nodeA_->rawValue() != nodeB_->rawValue())
			updateFlags_ |= Value;
		else
			updateFlags_ &= ~Value;

		if (nodeA_->type() != nodeB_->type())
			updateFlags_ |= Type;
		else
			updateFlags_ &= ~Type;
	}
}

void ChangeDescription::print() const
{
	if (nodeA_ || nodeB_)
		std::cout << (nodeA_ ? nodeA_->type() : nodeB_->type()).toStdString().c_str() << "\t";
	std::cout << id().toString().toStdString().c_str() << "\t";
	switch (type_)
	{
		case ChangeType::Insertion:
			std::cout << "Insertion" << std::endl;
			break;

		case ChangeType::Deletion:
			std::cout << "Deletion" << std::endl;
			break;

		case ChangeType::Move:
			std::cout << "Move" << std::endl;
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

	if (updateFlags_.testFlag(Label))
		std::cout << "\tLabel";
	if (updateFlags_.testFlag(Type))
		std::cout << "\tType";
	if (updateFlags_.testFlag(Value))
		std::cout << "\tValue";
	if (updateFlags_.testFlag(Structure))
		std::cout << "\tStructure";
	std::cout << std::endl;
}

void ChangeDescription::setStructureChangeFlag(bool value)
{
	if (value)
		updateFlags_ |= Structure;
	else
		updateFlags_ &= ~Structure;
}

} /* namespace FilePersistence */
