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

#pragma once

#include "../filepersistence_api.h"
#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class GenericTree;
class GenericNode;

enum class ChangeType {Unclassified, Insertion, Deletion, Move, Stationary};

class FILEPERSISTENCE_API ChangeDescription
{
	public:
		ChangeDescription(GenericNode* nodeA, GenericNode* nodeB);
		~ChangeDescription() = default;

		QString summary() const;

		static std::shared_ptr<ChangeDescription> newStructChange(
				Model::NodeIdType nodeId, std::shared_ptr<ChangeDescription> causingChange,
				std::shared_ptr<GenericTree> treeA, std::shared_ptr<GenericTree> treeB);

		/**
		 * If \a force is true then no check for ID uniqueness is performed.
		 */
		std::shared_ptr<ChangeDescription> copy(std::shared_ptr<GenericTree>& tree, bool force = false) const;

		enum UpdateType
		{
			NoFlags = 0,
			Label = 1,
			Value = 2,
			Type = 4,
			Structure = 8
		};
		using UpdateFlags = QFlags<UpdateType>;


		void computeFlags();
		bool hasFlags(const UpdateFlags flags) const;

		void print() const;

		void setStructureChangeFlag(bool value);

		Model::NodeIdType nodeId() const;

		const ChangeType& type() const;

		const UpdateFlags flags() const;

		GenericNode* nodeB() const;
		GenericNode* nodeA() const;

		/**
		 * This is only used for debugging. Do NOT use for other things.
		 */
		bool debugHasNodes();

		/**
		 * Returns \a true for changes that are stationary and have no flags set.
		 */
		bool isFake() const;
		bool onlyStructureChange() const;
		bool onlyLabelChange() const;

	private:
		ChangeDescription() = default;

		Model::NodeIdType nodeId_{};

		ChangeType type_{};

		UpdateFlags updateFlags_{};

		bool pointsToChildA_;
		bool pointsToChildB_;

		/**
		 * If pointsToChildA_ is true, this is a pointer to a node with parent whose ID is nodeId_.
		 * Otherwise, this points to a node with ID = nodeId_. This is a lazy loading mechanic.
		 */
		GenericNode* nodeA_{};
		GenericNode* nodeB_{};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ChangeDescription::UpdateFlags)

inline Model::NodeIdType ChangeDescription::nodeId() const { return nodeId_; }

inline bool ChangeDescription::hasFlags(const UpdateFlags flags) const { return (updateFlags_ & flags) == flags; }

inline const ChangeType& ChangeDescription::type() const { return type_; }

inline const ChangeDescription::UpdateFlags ChangeDescription::flags() const { return updateFlags_; }

inline bool ChangeDescription::isFake() const
{
	return type_ == ChangeType::Stationary && updateFlags_ == ChangeDescription::NoFlags;
}

inline bool ChangeDescription::onlyStructureChange() const {
	return (type_ == ChangeType::Stationary &&
			  (updateFlags_ == ChangeDescription::Structure ||
			  updateFlags_ == ChangeDescription::NoFlags));
}

inline bool ChangeDescription::onlyLabelChange() const {
	return (type_ == ChangeType::Stationary &&
			  (updateFlags_ == ChangeDescription::Label ||
			  updateFlags_ == ChangeDescription::NoFlags));
}

inline uint FILEPERSISTENCE_API qHash(const std::shared_ptr<const ChangeDescription> change, uint seed = 0)
{
	return ::qHash(change.get(), seed);
}

inline bool ChangeDescription::debugHasNodes() { return nodeA_ || nodeB_; }

}
