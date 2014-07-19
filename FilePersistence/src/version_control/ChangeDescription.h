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
		ChangeDescription(GenericNode* unchangedNode);
		ChangeDescription(GenericNode* oldNode, GenericNode* newNode);

		enum UpdateType
		{
			Order = 1,
			Value = 2,
			Type = 4,
			Children = 8
		};
		Q_DECLARE_FLAGS(UpdateFlags, UpdateType)

		static bool compareUpdateFlags(const UpdateFlags flags1, const UpdateFlags flags2);

		void print() const;

		void setChildrenUpdate(bool isUpdate);

		Model::NodeIdType id() const;

		const ChangeType& type() const;

		const UpdateFlags flags() const;

		const GenericNode* newNode() const;
		const GenericNode* oldNode() const;

	private:
		void fundamentalChangeClassification();

		void detectReorder();
		void detectValueUpdate();
		void detectTypeUpdate();

		ChangeType type_{};

		UpdateFlags updateFlags_;

		GenericNode* oldNode_{};
		GenericNode* newNode_{};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ChangeDescription::UpdateFlags)

inline const ChangeType& ChangeDescription::type() const { return type_; }

inline const ChangeDescription::UpdateFlags ChangeDescription::flags() const { return updateFlags_; }

inline const GenericNode* ChangeDescription::newNode() const { return newNode_; }
inline const GenericNode* ChangeDescription::oldNode() const { return oldNode_; }

} /* namespace FilePersistence */
