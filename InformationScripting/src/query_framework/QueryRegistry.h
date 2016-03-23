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

#include "../query_framework/Optional.h"

namespace Model {
	class Node;
}

namespace InformationScripting {

class Query;
class TupleSet;
class ArgumentRule;
class QueryExecutor;

class INFORMATIONSCRIPTING_API QueryRegistry
{
	public:
		static QueryRegistry& instance();

		template <typename QueryType, typename ...ForwardArguments>
		static void registerQuery(const QString& name, ForwardArguments... forwardArguments);

		enum class ExtraArguments : int { Default, QueryExecutor };
		template <typename QueryType, ExtraArguments extras, class ...ForwardArguments>
		static void registerQuery(const QString& name, ForwardArguments... forwardArguments);

		std::unique_ptr<Query> buildQuery(const QString& command, Model::Node* target,
													 QStringList args, QueryExecutor* executor);

		QStringList registeredQueries() const;
		QStringList scriptQueries() const;
		QStringList aliasQueries() const;

	private:
		QueryRegistry() = default;

		std::unique_ptr<Query> tryBuildQueryFromScript(const QString& name, Model::Node* target,
																	  QStringList args, QueryExecutor* executor);

		std::unique_ptr<Query> tryBuildQueryFromAlias(const QString& name, Model::Node* target,
																	 QStringList args, QueryExecutor* executor);

		using QueryConstructor = std::function<std::unique_ptr<Query> (Model::Node*, QStringList,
																							QueryExecutor*, QString)>;
		QHash<QString, QueryConstructor> constructors_;
		QString scriptLocation_{"scripts/"};
};

template <typename QueryType, typename ...ForwardArguments>
inline void QueryRegistry::registerQuery(const QString& name, ForwardArguments... forwardArguments)
{
	instance().constructors_[name] =
			[name, forwardArguments...] (Model::Node* target, QStringList args, QueryExecutor*, QString alias) {
				auto queryName = name;
				if (!alias.isNull()) queryName = alias;
				return std::unique_ptr<Query>(new QueryType{target, QStringList{queryName} + args, forwardArguments...});
	};
}

template <typename QueryType, QueryRegistry::ExtraArguments extras, class ...ForwardArguments>
inline void QueryRegistry::registerQuery(const QString& name, ForwardArguments... forwardArguments)
{
	if (extras == ExtraArguments::Default)
	{
		registerQuery<QueryType>(name, forwardArguments...);
	}
	else if (extras == ExtraArguments::QueryExecutor)
	{
		instance().constructors_[name] =
				[name, forwardArguments...] (Model::Node* target, QStringList args, QueryExecutor* executor, QString alias) {
					auto queryName = name;
					if (!alias.isNull()) queryName = alias;
					return std::unique_ptr<Query>(
								new QueryType{target, QStringList{queryName} + args, executor, forwardArguments...});
		};
	}
	else
		Q_ASSERT(false); // Invalid extra argument
}

inline QStringList QueryRegistry::registeredQueries() const { return constructors_.keys(); }

}
