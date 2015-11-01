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
#include "QueryPromptInput.h"
#include "QueryBuilder.h"

#include "../nodes/QueryNodeContainer.h"
#include "../queries/QueryExecutor.h"
#include "../queries/Query.h"

#include "VisualizationBase/src/items/Static.h"

#include "InteractionBase/src/prompt/Prompt.h"
#include "InteractionBase/src/commands/CommandResult.h"
#include "InteractionBase/src/vis/TextAndDescription.h"

namespace InformationScripting {

Visualization::Item* QueryPromptMode::createInputItem(const QString&)
{
	inputItem_ = new QueryPromptInput(nullptr);
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
	auto node = Interaction::Prompt::commandReceiver()->findAncestorWithNode()->node();
	Q_ASSERT(node);

	auto queryNode = inputItem_->query()->query();
	// Note a QueryExecutor should always be allocated with new, it is self destroying:
	auto executor = new QueryExecutor();
	QueryBuilder builder{node, executor};
	queryNode->accept(&builder);
	executor->addQuery(builder.query());
	auto result = executor->execute();

	if ( result->code() == Interaction::CommandResult::OK) Interaction::Prompt::hide();
	else
	{
		QList<Visualization::Item*> errorItems;

		for (auto& error : result->errors() )
		{
			if (error->visualization() == nullptr)
			{
				auto vis = new Interaction::TextAndDescription(nullptr,
											Interaction::TextAndDescription::itemStyles().get("command-prompt-error"));
				vis->setContents(error->message(), error->resolutionTips().join(" OR "));
				errorItems.append(vis);
			}
			else
			{
				//Extract the visualization
				errorItems.append(error->visualization());
				error->setVisualization(nullptr);
			}
		}

		showErrors(errorItems);
	}
}

}
