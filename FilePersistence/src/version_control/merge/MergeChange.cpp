/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
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

#include "MergeChange.h"

#include "../ChangeDescription.h"
#include "../../simple/GenericNode.h"

namespace FilePersistence {

MergeChange::MergeChange(ChangeType type, ChangeDescription::UpdateFlags updateFlags, Model::NodeIdType nodeId,
 Branches branches, Model::NodeIdType oldParentId, Model::NodeIdType newParentId,
 QString oldLabel, QString newLabel, QString oldType, QString newType, QString oldValue, QString newValue)
	: type_{type}, updateFlags_{updateFlags}, nodeId_{nodeId}, branches_{branches},
	 oldParentId_{oldParentId}, newParentId_{newParentId},
	 oldLabel_{oldLabel}, newLabel_{newLabel},
	 oldType_{oldType}, newType_{newType},
	 oldValue_{oldValue}, newValue_{newValue}
{
	Q_ASSERT(type_ != ChangeType::Unclassified);

	bool typeOrValueChange = updateFlags_ & (ChangeDescription::Value | ChangeDescription::Type);
	bool labelOrNonStationaryChange = type != ChangeType::Stationary || (updateFlags & ChangeDescription::Label);
	Q_ASSERT(typeOrValueChange != labelOrNonStationaryChange);
}

QList<MergeChange*> MergeChange::changesFromDiffChange(ChangeDescription& changeFromDiff, Branch branch)
{
	QList<MergeChange*> result;

	// Split the changes into Movement related and pure Type/Value update
	if (changeFromDiff.type() != ChangeType::Stationary || changeFromDiff.hasFlags(ChangeDescription::Label))
	{
		auto newFlags = changeFromDiff.flags() &  ChangeDescription::Label;
		result.append( new MergeChange{changeFromDiff.type(), newFlags, changeFromDiff.nodeId(), branch,
							changeFromDiff.nodeA() ? changeFromDiff.nodeA()->parentId() : Model::NodeIdType{},
							changeFromDiff.nodeB() ? changeFromDiff.nodeB()->parentId() : Model::NodeIdType{},
							changeFromDiff.nodeA() ? changeFromDiff.nodeA()->label() : QString{},
							changeFromDiff.nodeB() ? changeFromDiff.nodeB()->label() : QString{},
							{}, {}, {}, {}} );
	}

	if (changeFromDiff.hasFlags(ChangeDescription::Value) || changeFromDiff.hasFlags(ChangeDescription::Type))
	{
		auto newFlags = changeFromDiff.flags() & (ChangeDescription::Value | ChangeDescription::Type);
		result.append( new MergeChange{ChangeType::Stationary, newFlags, changeFromDiff.nodeId(), branch,
							{}, {}, {}, {},
							changeFromDiff.nodeA() ? changeFromDiff.nodeA()->type() : QString{},
							changeFromDiff.nodeB() ? changeFromDiff.nodeB()->type() : QString{},
							changeFromDiff.nodeA() ? changeFromDiff.nodeA()->rawValue() : QString{},
							changeFromDiff.nodeB() ? changeFromDiff.nodeB()->rawValue() : QString{}} );
	}

	return result;
}

}
