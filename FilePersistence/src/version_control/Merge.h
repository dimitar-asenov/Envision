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

#include "Diff.h"
#include "Commit.h"

namespace FilePersistence {

class GitRepository;

class FILEPERSISTENCE_API Merge
{
	public:
		~Merge();

		enum class Kind {Unclassified, AlreadyUpToDate, FastForward, TrueMerge};
		Kind kind() const;

		enum class Stage {NoMerge, Initialized, Classified, ConflictsDetected, ReadyToCommit, Complete};
		Stage stage() const;

		enum class Error {NoError, NoMergeBase};
		Error error() const;

		bool abort();
		bool commit(const Signature& author, const Signature& committer, const QString& message);

		std::unique_ptr<GenericTree> const& mergeTree() const;

	private:
		friend class GitRepository;

		enum class NodeSource {Base, HEAD, Revision};

		Merge(QString revision, bool fastForward, GitRepository* repository);

		void initialize(QString revision, bool fastForward, GitRepository* repository);

		void classifyKind();

		void performMerge();

		void performFastForward();

		void buildConflictUnitMap(IdToChangeDescriptionHash& CUToChange,
										  QHash<Model::NodeIdType, Model::NodeIdType>& changeToCU,
										  const Diff& diff, const std::unique_ptr<GenericTree>& versionTree,
										  const std::unique_ptr<GenericTree>& baseTree);

		Model::NodeIdType findConflicUnit(Model::NodeIdType nodeID, bool inBase, const Diff& diff,
													 const GenericPersistentUnit* unit) const;
		bool isConflictUnit(const GenericNode* node, NodeSource source, const ChangeDescription* baseToSource) const;
		bool isConflictUnitNode(const GenericNode*) const;

		QList<Model::NodeIdType> detectConflictingConflictUnits();

		void markConflictRegions(QList<Model::NodeIdType>& conflicts);

		struct Hunk
		{
				bool stable_;
				QList<Model::NodeIdType> head_;
				QList<Model::NodeIdType> revision_;
				QList<Model::NodeIdType> base_;

				Hunk(bool stable, QList<Model::NodeIdType> headList, QList<Model::NodeIdType> revisionList,
					  QList<Model::NodeIdType> baseList);
		};

		void computeMergeForLists(const std::unique_ptr<GenericTree>& head, const std::unique_ptr<GenericTree>& revision,
										  const std::unique_ptr<GenericTree>& base, const IdToChangeDescriptionHash& baseToHead,
										  const IdToChangeDescriptionHash& baseToRevision);

		int listInsertionIndex(const QList<Model::NodeIdType>& target, const QList<Model::NodeIdType>& current,
									  Model::NodeIdType insertID) const;

		QList<Model::NodeIdType> applyListMerge(const QList<Hunk>& hunkList, bool resolveOrder) const;

		QList<Hunk>& mergeLists(const QList<Model::NodeIdType> head, const QList<Model::NodeIdType> revision,
										const QList<Model::NodeIdType> base, Model::NodeIdType id);

		static QList<QList<Model::NodeIdType>> computeSublists(const QList<Model::NodeIdType> list,
																				 const QList<Model::NodeIdType> stableIDs);

		static QList<Model::NodeIdType> longestCommonSubsequence(const QList<Model::NodeIdType> listA,
																					const QList<Model::NodeIdType> listB);

		static QList<Model::NodeIdType> backtrackLCS(int** data, const QList<Model::NodeIdType> x,
																	const QList<Model::NodeIdType> y, int posX, int posY);

		static QList<Model::NodeIdType> genericNodeListToNodeIdList(const QList<GenericNode*>& list);

		enum class ListType {NoList, OrderedList, UnorderedList};
		static ListType getListType(const GenericNode* node);
		static bool isListType(const GenericNode* node);

		void loadGenericTree(const std::unique_ptr<GenericTree>& tree, const QString version);
		void findPersistentUnitDeclarations(GenericNode* node, IdToGenericNodeHash& declarations);

		void mergeChangesIntoTree(const std::unique_ptr<GenericTree>& tree, const IdToChangeDescriptionHash& changes,
										  QList<QSet<Model::NodeIdType>>& conflictRegions);
		void applyChangesToTree(const std::unique_ptr<GenericTree>& tree, const IdToChangeDescriptionHash& changes);


		bool applyAddToTree(const std::unique_ptr<GenericTree>& tree, IdToChangeDescriptionHash& changes,
								  const ChangeDescription* addOp);
		bool applyDeleteToTree(const std::unique_ptr<GenericTree>& tree, IdToChangeDescriptionHash& changes,
									  const ChangeDescription* deleteOp);
		bool applyMoveToTree(const std::unique_ptr<GenericTree>& tree, IdToChangeDescriptionHash& changes,
									const ChangeDescription* moveOp);
		bool applyStationaryChangeToTree(const std::unique_ptr<GenericTree>& tree, IdToChangeDescriptionHash& changes,
													const ChangeDescription* stationaryOp);

		void performInsertIntoList(GenericNode* parent, GenericNode* node);
		void performReorderInList(GenericNode* parent, GenericNode* node);

		static const QStringList ORDERED_LISTS;
		static const QStringList UNORDERED_LISTS;

		static const QStringList STATEMENTS;
		static const QStringList DECLARATIONS;
		static const QStringList ADDITIONAL_NODES;

		Kind kind_{};
		Stage stage_{};
		Error error_{};

		// List handling
		QHash<Model::NodeIdType, QList<Hunk>> mergedLists_;
		QSet<Model::NodeIdType> reorderedLists_;

		// Conflict regions
		QList<QSet<Model::NodeIdType>> conflictRegions_;
		QHash<Model::NodeIdType, QSet<Model::NodeIdType>&> nodeToRegionMap_;

		// Conflict units maps
		IdToChangeDescriptionHash revisionCUToChangeMap_;
		IdToChangeDescriptionHash headCUToChangeMap_;

		QHash<Model::NodeIdType, Model::NodeIdType> revisionChangeToCUMap_;
		QHash<Model::NodeIdType, Model::NodeIdType> headChangeToCUMap_;

		// GenericTrees
		std::unique_ptr<GenericTree> mergeBaseTree_;
		std::unique_ptr<GenericTree> revisionTree_;
		std::unique_ptr<GenericTree> headTree_;

		std::unique_ptr<GenericTree> mergeTree_;

		// Diffs to base version
		Diff baseRevisionDiff_;
		Diff baseHeadDiff_;

		bool fastForward_{};

		// Revisions
		QString head_;
		QString revision_;
		QString mergeBase_;

		GitRepository* repository_{};
};

inline Merge::Kind Merge::kind() const { return kind_; }
inline Merge::Stage Merge::stage() const { return stage_; }
inline Merge::Error Merge::error() const { return error_; }

} /* namespace FilePersistence */
