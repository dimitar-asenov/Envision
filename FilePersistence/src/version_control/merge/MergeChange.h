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

#pragma once

#include "../../filepersistence_api.h"

#include "../ChangeDescription.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class FILEPERSISTENCE_API MergeChange
{
	public:
		MergeChange(ChangeType type, ChangeDescription::UpdateFlags updateFlags, Model::NodeIdType nodeId,
			Model::NodeIdType oldParentId, Model::NodeIdType newParentId,
			QString oldLabel, QString newLabel, QString oldType, QString newType, QString oldValue, QString newValue);

		static QList<MergeChange> changesFromDiffChange(ChangeDescription& changeFromDiff);

		ChangeType type() const;
		ChangeDescription::UpdateFlags updateFlags() const;

		const Model::NodeIdType& nodeId() const;

		const Model::NodeIdType& oldParentId() const;
		const Model::NodeIdType& newParentId() const;

		const QString& oldLabel() const;
		const QString& newLabel() const;

		const QString& oldType() const;
		const QString& newType() const;

		const QString& oldValue() const;
		const QString& newValue() const;

		bool operator==(const MergeChange& other) const;
		bool operator!=(const MergeChange& other) const;

	private:
		ChangeType type_{};
		ChangeDescription::UpdateFlags updateFlags_{};

		Model::NodeIdType nodeId_;

		Model::NodeIdType oldParentId_;
		Model::NodeIdType newParentId_;

		QString oldLabel_;
		QString newLabel_;

		QString oldType_;
		QString newType_;

		QString oldValue_;
		QString newValue_;
};

inline ChangeType MergeChange::type() const { return type_; }
inline ChangeDescription::UpdateFlags MergeChange::updateFlags() const { return updateFlags_; }

inline const Model::NodeIdType& MergeChange::nodeId() const { return nodeId_; }
inline const Model::NodeIdType& MergeChange::oldParentId() const { return oldParentId_; }
inline const Model::NodeIdType& MergeChange::newParentId() const { return newParentId_; }
inline const QString& MergeChange::oldLabel() const { return oldLabel_; }
inline const QString& MergeChange::newLabel() const { return newLabel_; }
inline const QString& MergeChange::oldType() const { return oldType_; }
inline const QString& MergeChange::newType() const { return newType_; }
inline const QString& MergeChange::oldValue() const { return oldValue_; }
inline const QString& MergeChange::newValue() const { return newValue_; }

inline bool MergeChange::operator==(const MergeChange& other) const
{
	return type_ == other.type_ && updateFlags_ == other.updateFlags_ && nodeId_ == other.nodeId_
			&&	oldParentId_ == other.oldParentId_ && newParentId_ == other.newParentId_
			&& oldLabel_ == other.oldLabel_ && newLabel_ == other.newLabel_
			&& oldType_ == other.oldType_ && newType_ == other.newType_
			&& oldValue_ == other.oldValue_ && newValue_ == other.newValue_;
}

inline bool MergeChange::operator!=(const MergeChange& other) const { return !(*this == other);}

}
