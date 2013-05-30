/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "interactionbase_api.h"
#include "CommandPromptStyle.h"
#include "../commands/CommandSuggestion.h"
#include "../commands/CommandResult.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"

namespace Interaction {

class INTERACTIONBASE_API CommandPrompt : public Super<Visualization::Item>
{
	ITEM_COMMON(CommandPrompt)

	public:
		CommandPrompt(Visualization::Item* commandReceiver, const StyleType* style = itemStyles().get());
		virtual ~CommandPrompt();

		void setResult(CommandResult* result);
		void addSuggestion(CommandSuggestion* suggestion);
		void addSuggestions(const QList<CommandSuggestion*>& suggestions);

		void removeResult();
		void removeSuggestions();

		CommandResult* result();
		QList<CommandSuggestion*>& suggestions();

		Visualization::Item* commandReceiver();

		QString text() const;
		void takeSuggestion(CommandSuggestion* suggestion);

		void showPrompt();
		void hidePrompt();

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Item* commandReceiver_;
		Visualization::SequentialLayout* layout;
		Visualization::SequentialLayout* suggestionContainer;
		Visualization::SequentialLayout* errorContainer;
		Visualization::Text* command;

		int commandSelectedFirst;
		int commandSelectedLast;

		CommandResult* result_;
		QList<CommandSuggestion*> suggestions_;	//Suggestions from the result do not appear here.

		QPoint receiverCursorPosition;

		void saveReceiverCursorPosition();
		void setPromptPosition();
};

inline Visualization::Item* CommandPrompt::commandReceiver() { return commandReceiver_; }
inline QString CommandPrompt::text() const {return command->text();}

inline CommandResult* CommandPrompt::result() { return result_; }
inline QList<CommandSuggestion*>& CommandPrompt::suggestions() { return suggestions_; }

}
