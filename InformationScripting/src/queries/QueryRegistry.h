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

#include "../misc/Optional.h"

namespace Model {
	class Node;
}

namespace InformationScripting {

class Query;
class TupleSet;
class ArgumentRule;

class INFORMATIONSCRIPTING_API QueryRegistry
{
	public:
		static QueryRegistry& instance();

		using QueryConstructor = std::function<Query* (Model::Node*, QStringList)>;
		void registerQueryConstructor(const QString& command, QueryConstructor constructor);

		static void registerAlias(const QString& alias, const QString& aliasedQuery,
										  std::function<void (QStringList&)> argAdaption = {});

		template <class QueryType> using QueryExec = std::function<Optional<TupleSet> (QueryType*, TupleSet)>;

		template <class QueryType>
		static void registerQuery(const QString& name, QueryExec<QueryType> exec,
										  std::vector<ArgumentRule> argumentRules = {});

		Query* buildQuery(const QString& command, Model::Node* target, QStringList args);

		QStringList registeredQueries() const;
		QStringList scriptQueries() const;

	private:
		QueryRegistry() = default;

		Query* tryBuildQueryFromScript(const QString& name, Model::Node* target, QStringList args);

		QHash<QString, QueryConstructor> constructors_;
		QString scriptLocation_{"scripts/"};
};

inline void QueryRegistry::registerQueryConstructor(const QString& command, QueryRegistry::QueryConstructor constructor)
{
	Q_ASSERT(constructor);
	constructors_[command] = constructor;
}

template <class QueryType>
inline void QueryRegistry::registerQuery(const QString& name, QueryExec<QueryType> exec,
													  std::vector<ArgumentRule> argumentRules)
{
	instance().registerQueryConstructor(name, [name, exec, argumentRules](Model::Node* target, QStringList args) {
		return new QueryType(target, QStringList(name) + args, exec, argumentRules);
	});
}

inline QStringList QueryRegistry::registeredQueries() const { return constructors_.keys(); }

} /* namespace InformationScripting */
