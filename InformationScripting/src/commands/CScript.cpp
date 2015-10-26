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

#include "CScript.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/util/SymbolMatcher.h"

#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"

#include "../queries/QueryExecutor.h"
#include "../queries/CompositeQuery.h"
#include "../queries/SubstractOperator.h"
#include "../queries/NodePropertyAdder.h"
#include "../queries/UnionOperator.h"
#include "../queries/AddASTPropertiesAsTuples.h"
#include "../queries/QueryRegistry.h"
#include "../parsing/QueryParser.h"
#include "../parsing/QueryParsingException.h"

#include "../visualization/QueryPrompt.h"


namespace InformationScripting {

CScript::CScript() : Command{"script"} {}

bool CScript::canInterpret(Visualization::Item* source, Visualization::Item*, const QStringList& commandTokens,
									const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() > 1 && commandTokens.first() == "script" && source->findAncestorWithNode();
}

Interaction::CommandResult* CScript::execute(Visualization::Item* source, Visualization::Item*,
															const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	using namespace boost;

	auto node = source->findAncestorWithNode()->node();
	Q_ASSERT(node);

	QStringList args = commandTokens.mid(1);
	if (!args.size()) new Interaction::CommandResult();

	QString command = args[0];
	args = args.mid(1);

	if (command == "x")
	{
		new QueryPrompt(source);
	}
	else if (command != "fallback")
	{
		args.prepend(command);
		QueryExecutor queryExecutor;
		Query* q = nullptr;
		try {
			q = QueryParser::buildQueryFrom(args.join(""), node, &queryExecutor);
		} catch (const QueryParsingException& e) {
			return new Interaction::CommandResult(new Interaction::CommandError(e.message()));
		}
		queryExecutor.addQuery(q);
		return queryExecutor.execute();
	}
	return new Interaction::CommandResult();
}

} /* namespace InformationScripting */
