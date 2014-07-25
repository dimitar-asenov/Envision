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

#include "../simple/GenericTree.h"
#include "ChangeDescription.h"

namespace FilePersistence {

class GitRepository;

using IdToGenericNodeHash = QHash<Model::NodeIdType, GenericNode*>;
using IdToChangeDescriptionHash = QHash<Model::NodeIdType, ChangeDescription*>;

class FILEPERSISTENCE_API Diff
{
	public:
		Diff(QList<GenericNode*> oldNodes, GenericTree* oldTree,
			  QList<GenericNode*> newNodes, GenericTree* newTree,
			  const GitRepository* repository);

		~Diff();

		void print() const;

		IdToChangeDescriptionHash changes() const;
		IdToChangeDescriptionHash changes(ChangeType type) const;
		IdToChangeDescriptionHash changes(ChangeType type, ChangeDescription::UpdateFlags flags) const;

	private:
		void idMatching(IdToGenericNodeHash oldNodes, IdToGenericNodeHash newNodes);
		void findParentsInCommit(IdToGenericNodeHash nodes, GenericTree* tree, const GitRepository* repository);

		IdToChangeDescriptionHash changeDescriptions_;

		GenericTree* oldTree_{};
		GenericTree* newTree_{};
};

inline IdToChangeDescriptionHash Diff::changes() const {return changeDescriptions_;}

} /* namespace FilePersistence */
