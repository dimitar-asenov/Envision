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

#include "ConflictPipelineComponent.h"

namespace FilePersistence {

class ListMergeComponent : public ConflictPipelineComponent
{
	public:
		ListMergeComponent(QSet<QString>& listTypes);
		~ListMergeComponent();
		void run(const std::unique_ptr<GenericTree>&,
					const std::unique_ptr<GenericTree>&,
					const std::unique_ptr<GenericTree>&,
					ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
					QSet<ChangeDescription*>& conflictingChanges,
					ConflictPairs& conflictPairs);
	private:
		bool onlyChildStructure(ChangeDescription* change);
		bool onlyLabel(ChangeDescription* change);
		QSet<QString> listTypes_;
		QSet<const GenericNode *> listsToMerge_;

		// Below are things taken over from the old merge because they might still be useful.

		struct Chunk
		{
				bool stable_;
				QList<Model::NodeIdType> head_;
				QList<Model::NodeIdType> revision_;
				QList<Model::NodeIdType> base_;

				Chunk(bool stable, QList<Model::NodeIdType> headList, QList<Model::NodeIdType> revisionList,
					  QList<Model::NodeIdType> baseList);
		};
		void computeMergeForLists(const std::unique_ptr<GenericTree>& head, const std::unique_ptr<GenericTree>& revision,
										  const std::unique_ptr<GenericTree>& base, const IdToChangeDescriptionHash& baseToHead,
										  const IdToChangeDescriptionHash& baseToRevision);


		QList<Chunk>& computeMergeChunks(const QList<Model::NodeIdType> idListA, const QList<Model::NodeIdType> idListB,
													const QList<Model::NodeIdType> idListBase, Model::NodeIdType containerId);

		static QList<Model::NodeIdType> backtrackLCS(int** data, const QList<Model::NodeIdType> x,
																	const QList<Model::NodeIdType> y, int posX, int posY);

		int listInsertionIndex(const QList<Model::NodeIdType>& target, const QList<Model::NodeIdType>& current,
									  Model::NodeIdType insertID) const;

		bool applyListMerge(QList<Model::NodeIdType>& mergedList, const QList<Chunk>& chunkList, bool resolveOrder) const;

		static QList<QList<Model::NodeIdType>> computeSublists(const QList<Model::NodeIdType> list,
																				 const QList<Model::NodeIdType> stableIDs);

		static QList<Model::NodeIdType> longestCommonSubsequence(const QList<Model::NodeIdType> listA,
																					const QList<Model::NodeIdType> listB);

		void performInsertIntoList(GenericNode* parent, GenericNode* node);
		void performReorderInList(GenericNode* parent, GenericNode* node);

		// List handling
		QHash<Model::NodeIdType, QList<Chunk>> mergedLists_;
		QSet<Model::NodeIdType> reorderedLists_;
};

inline bool ListMergeComponent::onlyChildStructure(ChangeDescription* change) {
	return (change->type() == ChangeType::Stationary &&
			  (change->flags() == ChangeDescription::Structure ||
			  change->flags() == ChangeDescription::NoFlags));
}

inline bool ListMergeComponent::onlyLabel(ChangeDescription* change) {
	return (change->type() == ChangeType::Stationary &&
			  (change->flags() == ChangeDescription::Label ||
			  change->flags() == ChangeDescription::NoFlags));
}

} /* namespace FilePersistence */
