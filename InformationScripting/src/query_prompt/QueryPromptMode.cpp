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
#include "QueryPromptMode.h"

#include "nodes/QueryNodeContainer.h"
#include "parsing/QueryBuilder.h"
#include "visualization/QueryPromptInput.h"
#include "../queries/Query.h"
#include "../query_framework/QueryExecutor.h"
#include "../query_framework/QueryParsingException.h"
#include "../query_framework/QueryResultVisualizer.h"

#include "VisualizationBase/src/items/Static.h"

#include "InteractionBase/src/prompt/Prompt.h"

namespace InformationScripting {

Visualization::Item* QueryPromptMode::createInputItem(const QString&)
{
	QueryResultVisualizer::cleanScene();
	inputItem_ = new QueryPromptInput{nullptr};
	return inputItem_;
}

Visualization::StaticStyle* QueryPromptMode::modeIcon() const
{
	return Visualization::Static::itemStyles().get("query-prompt-mode");
}

void QueryPromptMode::setSelection(InputSelection selection)
{
	inputItem_->setSelection(selection);
}

void QueryPromptMode::onEnterKeyPress(Qt::KeyboardModifiers)
{
	QList<QString> errors;

	auto firsAncestosterWithNode = Interaction::Prompt::commandReceiver()->findAncestorWithNode();
	auto node = firsAncestosterWithNode ? firsAncestosterWithNode->node() : nullptr;

	if (node)
	{
		auto queryNode = inputItem_->query()->query();
		// Note a QueryExecutor should always be allocated with new, it is self destroying.
		// The QueryBuilder below might however throw an exception when building the queries, if that is the case
		// The executor will not execute and thus will never delete itself, thus we have to manually destroy it
		// in the catch code.
		auto executor = new QueryExecutor{};
		QueryBuilder builder{node, executor};
		try
		{
			auto queries = builder.visit(queryNode);
			for (auto& query : queries)
				executor->addQuery(std::move(query));
			errors = executor->execute();
		}
		catch (const QueryParsingException& parsingException)
		{
			errors = {parsingException.message()};
			SAFE_DELETE(executor);
		}
	}
	else
		errors = {"Queries only work on nodes"};

	if (errors.isEmpty())
	{
		// Remove the scene cursor so that nothing is selected.
		Interaction::Prompt::commandReceiver()->scene()->setMainCursor(nullptr);

		Interaction::Prompt::hide();
	}
	else showErrors(errors, Qt::RichText);
}

}
