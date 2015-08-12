/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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
		CommandPrompt(Visualization::Item* commandReceiver, QString initialCommandText = QString(),
				bool centerOnPrompt = false, const StyleType* style = itemStyles().get());
		virtual ~CommandPrompt();

		void setResult(QSharedPointer<CommandResult> result);
		void addSuggestions(const QList<CommandSuggestion*>& suggestions);

		void removeResult();
		void removeSuggestions();

		QSharedPointer<CommandResult> result();
		QList<CommandSuggestion*>& suggestions();

		Visualization::Item* commandReceiver();

		QString text() const;
		void takeSuggestion(CommandSuggestion* suggestion);
		void executeCurrentText();
		const std::unique_ptr<Visualization::Cursor>& commandReceiverCursor();

		void showPrompt(QString initialCommandText = QString(), bool centerOnPrompt = false);
		void hidePrompt();
		void cancelPrompt();

		bool wasCancelled() const;

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	private:
		Visualization::Item* commandReceiver_{};
		std::unique_ptr<Visualization::Cursor> commandReceiverCursor_{};

		Visualization::SequentialLayout* layout_{};
		Visualization::SequentialLayout* errorContainer_{};
		Visualization::Text* command_{};

		int commandSelectedFirst_{};
		int commandSelectedLast_{};

		bool hideRequested_{};
		bool wasCancelled_{}; // Set to true when the prompt was hidden manually by the user

		QSharedPointer<CommandResult> result_;
		QList<CommandSuggestion*> suggestions_;

		void saveReceiverCursor();
		void setPromptPosition();
		void showAutocompleteBasedOnSuggestions();
		QPoint receiverCursorPosition();

		void centerViewOnPrompt() const;

		static const QString TYPE_HINT;
};

inline Visualization::Item* CommandPrompt::commandReceiver() { return commandReceiver_; }
inline QString CommandPrompt::text() const {return command_->text();}
inline const std::unique_ptr<Visualization::Cursor>& CommandPrompt::commandReceiverCursor()
{ return commandReceiverCursor_;}

inline QSharedPointer<CommandResult> CommandPrompt::result() { return result_; }
inline QList<CommandSuggestion*>& CommandPrompt::suggestions() { return suggestions_; }
inline bool CommandPrompt::wasCancelled() const { return wasCancelled_; }
inline void CommandPrompt::cancelPrompt() {wasCancelled_ = true; hidePrompt();}

}
