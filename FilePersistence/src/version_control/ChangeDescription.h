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

#pragma once

#include "../simple/GenericNode.h"

namespace FilePersistence {

enum class ChangeType {Unclassified, Added, Deleted, Moved, Stationary};

class FILEPERSISTENCE_API ChangeDescription
{
	public:
		ChangeDescription(GenericNode* nodeA, GenericNode* nodeB);
		ChangeDescription(ChangeType type);

		enum UpdateType
		{
			Label = 1,
			Value = 2,
			Type = 4,
			Children = 8
		};
		Q_DECLARE_FLAGS(UpdateFlags, UpdateType)

		bool hasFlags(const UpdateFlags flags) const;

		void print() const;

		void setChildrenUpdate(bool isUpdate);

		Model::NodeIdType id() const;

		const ChangeType& type() const;

		const UpdateFlags flags() const;

		const GenericNode* nodeB() const;
		const GenericNode* nodeA() const;

	private:
		void fundamentalChangeClassification();

		void detectReorder();
		void detectValueUpdate();
		void detectTypeUpdate();

		ChangeType type_{};

		UpdateFlags updateFlags_;

		GenericNode* nodeA_{};
		GenericNode* nodeB_{};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ChangeDescription::UpdateFlags)

inline bool ChangeDescription::hasFlags(const UpdateFlags flags) const { return (updateFlags_ & flags) == flags; }

inline const ChangeType& ChangeDescription::type() const { return type_; }

inline const ChangeDescription::UpdateFlags ChangeDescription::flags() const { return updateFlags_; }

inline const GenericNode* ChangeDescription::nodeB() const { return nodeB_; }
inline const GenericNode* ChangeDescription::nodeA() const { return nodeA_; }

} /* namespace FilePersistence */
