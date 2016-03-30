/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
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

#include "../informationscripting_api.h"

#include "LinearQuery.h"
#include "../query_framework/ArgumentParser.h"

namespace FilePersistence {
	struct CommitMetaData;
	class GitRepository;
}

namespace Model {
	class Node;
	class TreeManager;
}

namespace InformationScripting {

class INFORMATIONSCRIPTING_API VersionControlQuery : public LinearQuery
{
	public:
		virtual Optional<TupleSet> executeLinear(TupleSet input) override;

		static void registerDefaultQueries();
	private:
		friend class QueryRegistry;

		ArgumentParser arguments_;

		static const QStringList COUNT_ARGUMENT_NAMES;
		static const QStringList NODE_TYPE_ARGUMENT_NAMES;
		static const QStringList NODES_ARGUMENTS_NAMES;
		static const QStringList IN_ARGUMENT_NAMES;
		static const QStringList INCLUDE_INTERMEDIATE_VERSIONS_ARGUMENT_NAMES;
		static const QStringList TYPED_CHANGES_ARGUMENT_NAMES;

		VersionControlQuery(Model::Node* target, QStringList args, std::vector<ArgumentRule> argumentRules);

		static void addCommitMetaInformation(TupleSet& ts, const FilePersistence::CommitMetaData& metadata);

		Optional<QList<QString>> commitsToConsider(const QStringList& commitIdRange) const;
};

}
