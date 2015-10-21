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
#include "../parsing/ArgumentParser.h"

#include "ModelBase/src/nodes/Node.h"

namespace Model {
	class SymbolMatcher;
}

namespace OOModel {
	class Class;
	class Expression;
	class Method;
}

namespace InformationScripting {

class INFORMATIONSCRIPTING_API AstQuery : public LinearQuery
{
	public:
		virtual Optional<TupleSet> executeLinear(TupleSet input) override;

		static void registerDefaultQueries();

	private:
		Model::Node* target_{};
		std::unique_ptr<ArgumentParser> arguments_{};

		static const QStringList NODETYPE_ARGUMENT_NAMES;
		static const QStringList NAME_ARGUMENT_NAMES;
		static const QStringList ADD_AS_NAMES;
		static const QStringList ATTRIBUTE_NAME_NAMES;

		using ExecuteFunction = std::function<Optional<TupleSet> (AstQuery*, TupleSet)>;
		ExecuteFunction exec_{};

		AstQuery(ExecuteFunction exec, Model::Node* target, QStringList args,
					std::vector<ArgumentRule> argumentRules);

		static void setTypeTo(QStringList& args, QString type);

		Optional<TupleSet> baseClassesQuery(TupleSet input);
		Optional<TupleSet> toParentType(TupleSet input);
		Optional<TupleSet> callGraph(TupleSet input);
		Optional<TupleSet> genericQuery(TupleSet input);
		Optional<TupleSet> typeQuery(TupleSet input, QString type);
		Optional<TupleSet> nameQuery(TupleSet input, QString name);
		Optional<TupleSet> usesQuery(TupleSet input);
		Optional<TupleSet> typeFilter(TupleSet input);
		Optional<TupleSet> attribute(TupleSet input);

		void addBaseEdgesFor(OOModel::Class* childClass, NamedProperty& classNode, TupleSet& ts);
		void addNodesOfType(TupleSet& ts, const Model::SymbolMatcher& matcher, Model::Node* from = nullptr);
		template <class Predicate>
		void addNodesForWhich(TupleSet& ts, Predicate holds, Model::Node* from = nullptr);
		static void addCallInformation(TupleSet& ts, OOModel::Method* method, QList<OOModel::Method*> callees);

		void adaptOutputForRelation(TupleSet& tupleSet, const QString& relationName, const QStringList& keepProperties);

		static bool matchesExpectedType(Model::Node* node, Model::Node::SymbolType symbolType,
										 const QString& expectedType, const QStringList& args);

		static void registerQuery(const QString& name, ExecuteFunction methodToCall,
										  std::vector<ArgumentRule> argumentRules = {}, const QString& setTypeTo = {});
};

} /* namespace InformationScripting */
