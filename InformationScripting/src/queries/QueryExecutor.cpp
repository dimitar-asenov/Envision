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

#include "QueryExecutor.h"

#include "TopLevelQuery.h"

#include "visualization/DefaultVisualizer.h"

#include "InteractionBase/src/commands/CommandResult.h"

namespace InformationScripting {

QueryExecutor::~QueryExecutor()
{
	Q_ASSERT(queries_.empty());
	qDebug() << "DELETE QUERYEXECUTOR";
}

void QueryExecutor::addQuery(std::unique_ptr<TopLevelQuery>&& query)
{
	queries_.emplace(std::move(query));
}

Interaction::CommandResult* QueryExecutor::execute()
{
	Q_ASSERT(!queries_.empty());

	bool hasError = false;
	QString errorMessage{};

	auto query = std::move(queries_.front());
	queries_.pop();

	auto results = query->execute({});
	if (results.size())
	{
		// TODO how to handle warnings? CommandResult has no warnings?
		if (results[0].hasWarnings())
			qWarning() << results[0].warnings();
		if (results[0])
		{
			auto val = results[0].value();
			DefaultVisualizer::instance().visualize(val);
			results.clear();
		}
		else
		{
			hasError = true;
			errorMessage = results[0].errors()[0];
		}
	}

	if (hasError)
		return new Interaction::CommandResult(new Interaction::CommandError(errorMessage));
	else
		return new Interaction::CommandResult();
}

} /* namespace InformationScripting */
