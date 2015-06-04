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

using IdToChangeMultiHash = QMultiHash<Model::NodeIdType, std::shared_ptr<const ChangeDescription>>;

class ConflictUnitDetector : public ConflictPipelineComponent
{
	public:
		ConflictUnitDetector(QSet<QString>& conflictTypes, QString revisionIdA, QString revisionIdB, QString revisionIdBase);
		~ConflictUnitDetector();
		LinkedChangesTransition run(ChangeDependencyGraph& cdgA, ChangeDependencyGraph& cdgB,
					QSet<std::shared_ptr<const ChangeDescription> >& conflictingChanges,
					ConflictPairs& conflictPairs, LinkedChangesSet& linkedChangesSet);
	private:
		/**
		 * Returns a MultiHash where the key set contains an ID if and only if it is the ID of a conflict unit root node
		 * and that conflict unit is affected by some change in \a cdg. The \a revision argument is only used to load nodes
		 * that are not yet loaded. It would be prefered if this wasn't necessary.
		 */
		IdToChangeMultiHash computeAffectedCUs(const QString revision,
															ChangeDependencyGraph cdg);
		/**
		 * Returns the ID of the root of the conflict unit \a node belongs to. \a nodeRevision is the revision \a node comes
		 * from (one of the branches, not the base). Both the \a nodeRevision and \a cdg arguments are only used to load nodes
		 * and could be removed if node loading is implemented transparently.
		 */
		Model::NodeIdType findConflictUnit(const GenericNode* node, const QString nodeRevision,
													  const ChangeDependencyGraph& cdg);
		/**
		 * This is a helper function for \a findConflictUnit(). It should become obsolete if node loading is implemented
		 * transparently.
		 *
		 * \a base should be true when \a revision is the base revision.
		 */
		const GenericNode* getParent(const GenericNode* node, bool base, const QString revision,
											  const ChangeDependencyGraph& cdg);
		/**
		 * This is a helper function for \a findConflictUnit(). It should become obsolete if node loading is implemented
		 * transparently.
		 *
		 * Returns \a nullptr if the node does not exist in base.
		 */
		const GenericNode* getInBase(const GenericNode* node, const ChangeDependencyGraph& cdg);


		QSet<QString> conflictTypes_;
		IdToChangeMultiHash affectedCUsA_;
		IdToChangeMultiHash affectedCUsB_;

		QString revisionIdA_;
		QString revisionIdB_;
		QString revisionIdBase_;
};

} /* namespace FilePersistence */
