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

#include "RuntimeQuery.h"

#include "../query_framework/QueryRegistry.h"
#include "../query_framework/QueryExecutor.h"

#include "InteractionBase/src/commands/CommandResult.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/model/TreeManager.h"

#include "OODebug/src/debugger/JavaDebugger.h"
#include "OODebug/src/debugger/jdwp/messages/EventSet.h"

namespace InformationScripting {

Optional<TupleSet> RuntimeQuery::executeLinear(TupleSet input)
{
	auto execCount = std::make_shared<std::unordered_map<Model::Node*, int>>();
	auto listener = [execCount](Model::Node* target, auto...) {
		++(*execCount)[target];
		return false;
	};

	auto callbackIds = std::make_shared<std::vector<int>>();
	for (const auto& breakpointTuple : input.tuples("breakpoint"))
		callbackIds->push_back(
					OODebug::JavaDebugger::instance().addBreakpointListener(breakpointTuple["breakpoint"], listener));

	auto manager = target()->manager();
	Q_ASSERT(manager);
	std::unique_ptr<Interaction::CommandResult> executionResult
		{OODebug::JavaDebugger::instance().debugTree(manager, "exported/" + manager->root()->symbolName())};
	if (executionResult->code() == Interaction::CommandResult::Error)
		return {"errror during exection"};

	auto queryExecutor = executor_;
	Q_ASSERT(queryExecutor);
	OODebug::JavaDebugger::instance().addProgramExitLister([callbackIds, execCount, queryExecutor]()
		{
			for (int id : *callbackIds)
				OODebug::JavaDebugger::instance().removeBreakpointListener(id);

			TupleSet result;
			for (const auto& val : *execCount)
				result.add({{"count", val.second}, {"ast", val.first}});

			queryExecutor->execute({result});
		}
	);
	return TupleSet{};
}

void RuntimeQuery::registerDefaultQueries()
{
	QueryRegistry::registerQuery<RuntimeQuery, QueryRegistry::ExtraArguments::QueryExecutor>("traceExecution");
}

RuntimeQuery::RuntimeQuery(Model::Node* target, QStringList, QueryExecutor* executor)
	: LinearQuery{target}, executor_{executor}
{}

}
