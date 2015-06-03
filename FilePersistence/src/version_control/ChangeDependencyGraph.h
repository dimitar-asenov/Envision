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

#include "ChangeDescription.h"
#include "Diff.h"

namespace FilePersistence {

class ChangeDependencyGraph
{
	public:
		ChangeDependencyGraph(Diff& diff);
		~ChangeDependencyGraph();
		const IdToChangeDescriptionHash changes() const;
		void insert(std::shared_ptr<const ChangeDescription>& change);
		void remove(std::shared_ptr<const ChangeDescription>& change);
		void replace(std::shared_ptr<const ChangeDescription>& oldChange,
						 std::shared_ptr<const ChangeDescription>& newChange);
		/**
		 * Records the fact that \a changeA depends on changeB.
		 */
		void addDependency(std::shared_ptr<const ChangeDescription>& changeA,
								 std::shared_ptr<const ChangeDescription>& changeB);
		/**
		 * Records the fact that \a changeA no longer depends on changeB.
		 */
		void removeDependency(std::shared_ptr<const ChangeDescription>& changeA,
									 std::shared_ptr<const ChangeDescription>& changeB);
	private:
		void recordDependencies(std::shared_ptr<const ChangeDescription> change, bool incoming);
		IdToChangeDescriptionHash changes_;
		/**
		 * A mapping of changeA to changeB means changeA depends on changeB.
		 */
		QMultiHash<std::shared_ptr<const ChangeDescription>, std::shared_ptr<const ChangeDescription>> dependencies_;
		// NOTE if this were a "two-way" hash, we could gain some speed.
};

inline const IdToChangeDescriptionHash ChangeDependencyGraph::changes() const { return changes_; }

} /* namespace FilePersistence */
