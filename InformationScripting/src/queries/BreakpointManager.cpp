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

#include "BreakpointManager.h"

#include "../query_framework/QueryRegistry.h"

#include "OODebug/src/debugger/JavaDebugger.h"
#include "OOModel/src/statements/ExpressionStatement.h"

namespace InformationScripting {

const QStringList BreakpointManager::VISIBLE_ARGUMENT_NAMES{"v", "visible"};

Optional<TupleSet> BreakpointManager::executeLinear(TupleSet input)
{
	auto tuples = input;
	auto type = OODebug::JavaDebugger::BreakpointType::Internal;
	if (arguments_.argument(VISIBLE_ARGUMENT_NAMES[0]) != "no")
		type = OODebug::JavaDebugger::BreakpointType::User;

	for (const auto& t : tuples.tuples("ast"))
	{
		for (auto target : Model::Node::childrenOfType<OOModel::ExpressionStatement>(t["ast"]))
		{
			qint64 breakpointId = OODebug::JavaDebugger::instance().addBreakpoint(target, type);
			tuples.add({{"breakpoint", breakpointId}, {"visible", arguments_.argument(VISIBLE_ARGUMENT_NAMES[0])}});
		}
	}
	return tuples;
}

void BreakpointManager::registerDefaultQueries()
{
	QueryRegistry::registerQuery<BreakpointManager>("addBreakpoints");
}

BreakpointManager::BreakpointManager(Model::Node* target, QStringList args)
	: LinearQuery{target}, arguments_{{
	{VISIBLE_ARGUMENT_NAMES, "Wether the breakpoint is visible, default = no", VISIBLE_ARGUMENT_NAMES[1], "no"}
}, args}
{}


}
