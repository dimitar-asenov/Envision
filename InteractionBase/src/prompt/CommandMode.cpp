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

#include "CommandMode.h"
#include "PromptTextInput.h"
#include "Prompt.h"

#include "../autocomplete/AutoComplete.h"
#include "../handlers/GenericHandler.h"
#include "../commands/CommandExecutionEngine.h"
#include "../commands/CommandSuggestion.h"
#include "../commands/CommandResult.h"
#include "../vis/TextAndDescription.h"

#include "VisualizationBase/src/items/Static.h"

namespace Interaction {

const QString TYPE_HINT = "Type a command";

Visualization::Item* CommandMode::createInputItem(const QString& initialCommandText)
{
	auto commandText = initialCommandText;
	if (commandText.isNull()) commandText = TYPE_HINT;
	inputItem_ = new PromptTextInput{nullptr, commandText};
	return inputItem_;
}

Visualization::StaticStyle* CommandMode::modeIcon() const
{
	return Visualization::Static::itemStyles().get("command-prompt-command-mode");
}

void CommandMode::setSelection(InputSelection selection)
{
	inputItem_->setSelection(selection);
}

void CommandMode::onShellUpdate()
{
	updateSuggestions();
}

void CommandMode::updateSuggestions()
{
	removeSuggestions();
	QString text = inputItem_->text() == TYPE_HINT ? "" : inputItem_->text();
	addSuggestions( GenericHandler::executionEngine()->autoComplete(
							 Prompt::commandReceiver(), text, Prompt::commandReceiverCursor()));
}

void CommandMode::addSuggestions(QList<CommandSuggestion*> suggestions)
{
	for (auto s : suggestions)
		suggestions_.emplace_back(s);
	showAutocompleteBasedOnSuggestions();
}

void CommandMode::removeSuggestions()
{
	suggestions_.clear();
	AutoComplete::hide();
}

void CommandMode::showAutocompleteBasedOnSuggestions()
{
	auto executeFunction = [this](AutoCompleteEntry* e){
		if (Prompt::mode() == this)
		{
			takeSuggestion(static_cast<CommandSuggestion*>(e));
			onEnterKeyPress(Qt::NoModifier);
		}
	};

	QList<AutoCompleteEntry*> entries;
		for (auto& s : suggestions_) entries.append(new AutoCompleteEntry{s->text(), s->description(),
				s->visualization(), executeFunction});

	if (entries.isEmpty() || Prompt::mode() != this)
		AutoComplete::hide();
	else
		AutoComplete::show(entries, true);
}

void CommandMode::takeSuggestion(CommandSuggestion* suggestion)
{
	inputItem_->setText(suggestion->text());
	inputItem_->setSelection(PromptMode::AtEnd);
}

void CommandMode::onTabKeyPress(Qt::KeyboardModifiers)
{
	if (suggestions_.size() == 1) takeSuggestion(suggestions_[0].get());
}

void CommandMode::onEnterKeyPress(Qt::KeyboardModifiers)
{
	Prompt::commandReceiver()->execute( inputItem_->text(), Prompt::commandReceiverCursor());

	auto result = GenericHandler::executionEngine()->result();

	if ( result->code() == CommandResult::OK) Prompt::hide();
	else
	{
		QList<Visualization::Item*> errorItems;

		for (auto& error : result->errors() )
		{
			if (error->visualization() == nullptr)
			{
				auto vis = new TextAndDescription{nullptr, TextAndDescription::itemStyles().get("command-prompt-error")};
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
