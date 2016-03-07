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

#include "ChangeDependencyGraph.h"
#include "ConflictPipelineComponent.h"
#include "LinkedChangesTransition.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class GenericNode;
class GenericTree;
class ChangeDescription;
class LinkedChangesSet;
class ConflictPairs;

using ConflictUnitSet = QMultiHash<Model::NodeIdType, std::shared_ptr<ChangeDescription>>;

class FILEPERSISTENCE_API ConflictUnitDetector : public ConflictPipelineComponent
{
	public:
		ConflictUnitDetector(QSet<QString>& conflictTypes,
									bool useLinkedChanges);
		~ConflictUnitDetector();
		LinkedChangesTransition run(std::shared_ptr<GenericTree>& treeA,
											 std::shared_ptr<GenericTree>& treeB,
											 std::shared_ptr<GenericTree>& treeBase,
											 ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
					QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
					ConflictPairs& conflictPairs, LinkedChangesSet& linkedChangesSet);
	private:
		/**
		 * Returns a MultiHash where the key set contains an ID if and only if it is the ID of a conflict unit root node
		 * and that conflict unit is affected by some change in \a cdg. The \a revision argument is only used to load nodes
		 * that are not yet loaded. It would be prefered if this wasn't necessary.
		 */
		ConflictUnitSet computeAffectedCUs(ChangeDependencyGraph cdg);
		/**
		 * Returns the ID of the root of the conflict unit \a node belongs to.
		 */
		Model::NodeIdType findConflictUnit(const GenericNode* node);

		static QSet<std::shared_ptr<ChangeDescription>> structureChanges(ChangeDependencyGraph& cdg,
																					 std::shared_ptr<ChangeDescription> parentChange);


		static Model::NodeIdType findMoveBound (GenericNode* nodeA, GenericNode* nodeB);

		/**
		 * Returns all changes \a change depends on.
		 */
		static QList<std::shared_ptr<ChangeDescription>> findMoveDependencies(std::shared_ptr<GenericTree> treeBase,
																							std::shared_ptr<ChangeDescription> change,
																							ChangeDependencyGraph& cdg);

		static void checkMove(std::shared_ptr<ChangeDescription> changeA, ChangeDependencyGraph& cdgB,
									 ConflictPairs& conflictPairs, QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges);


		static void checkMoves(ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
				ConflictPairs& conflictPairs, QSet<std::shared_ptr<ChangeDescription>>& conflictingChanges);

		static bool isAncestor(GenericNode* node, Model::NodeIdType ancestorId);

		/**
		 * Adds all changes that depend on \a change according to \a cdg to \a conflictingChanges.
		 */
		void markDependingAsConflicting(QSet<std::shared_ptr<ChangeDescription> >& conflictingChanges,
												  std::shared_ptr<ChangeDescription> change, ChangeDependencyGraph& cdg,
												  QMultiHash<std::shared_ptr<ChangeDescription>, std::shared_ptr<ChangeDescription> >& moveDependencies,
												  QList<std::shared_ptr<ChangeDescription> > conflictingWithChange, ConflictPairs& conflictPairs);

		QSet<QString> conflictTypes_;
		ConflictUnitSet affectedCUsA_;
		ConflictUnitSet affectedCUsB_;
		bool useLinkedChanges_;

		std::shared_ptr<GenericTree> treeBase_;
};

}
