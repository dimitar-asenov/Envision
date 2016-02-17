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

#include "../queries/Query.h"
#include "QueryResultVisualizer.h"
#include "QueryRegistry.h"

#include "InteractionBase/src/commands/CommandResult.h"

#include "VisualizationBase/src/VisualizationManager.h"

namespace InformationScripting {

QueryExecutor::QueryExecutor(Model::Node* target) : target_{target}
{}

QueryExecutor::~QueryExecutor()
{
	Q_ASSERT(queries_.empty());
}

void QueryExecutor::addQuery(std::unique_ptr<Query>&& query)
{
	queries_.emplace(std::forward<std::unique_ptr<Query>>(query));
}

QList<QString> QueryExecutor::execute(const QList<TupleSet>& input)
{
	Q_ASSERT(!queries_.empty());

	QList<QString> errorMessages;

	auto query = std::move(queries_.front());
	queries_.pop();

	auto results = query->execute(input);
	if (queries_.empty() && results.size())
	{
		// TODO how to handle warnings? CommandResult has no warnings?
		if (results[0].hasWarnings())
			qWarning() << results[0].warnings();
		if (results[0])
		{
			if (defaultVisualize_)
			{
				auto vis = QueryRegistry::instance().buildQuery("show", target_, {}, this);
				vis->execute({results[0].value()});
				results.clear();
			}
		}
		else
			errorMessages = results[0].errors();
	}

	if (queries_.empty())
	{
		// deleteLater
		QApplication::postEvent(Visualization::VisualizationManager::instance().mainScene(),
																 new Visualization::CustomSceneEvent{[this](){delete this;}});
	}

	return errorMessages;
}

}
