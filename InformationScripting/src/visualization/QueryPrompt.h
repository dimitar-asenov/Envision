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

#pragma once

#include "../informationscripting_api.h"

#include "InteractionBase/src/commands/CommandSuggestion.h"
#include "InteractionBase/src/commands/CommandResult.h"

#include "VisualizationBase/src/declarative/DeclarativeItem.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"

namespace InformationScripting {

class QueryNodeContainer;

class INFORMATIONSCRIPTING_API QueryPrompt : public Super<Visualization::DeclarativeItem<QueryPrompt>>
{
	ITEM_COMMON_CUSTOM_STYLENAME(QueryPrompt, Visualization::DeclarativeItemBaseStyle)

	public:
		QueryPrompt(Visualization::Item* commandReceiver, QString initialCommandText = QString(),
				bool centerOnPrompt = false, const StyleType* style = itemStyles().get());
		virtual ~QueryPrompt();

		static void initializeForms();

		Visualization::Item* commandReceiver();

		QString text() const;
		void executeCurrentText();
		const std::unique_ptr<Visualization::Cursor>& commandReceiverCursor();

		void showPrompt(QString initialCommandText = QString(), bool centerOnPrompt = false);
		void hidePrompt();
		void cancelPrompt();

		bool wasCancelled() const;

	private:
		Visualization::Item* commandReceiver_{};
		std::unique_ptr<Visualization::Cursor> commandReceiverCursor_{};

		Visualization::Text* errorText_{};

		QueryNodeContainer* queryContainer_{};
		Visualization::Item* queryVis_{};

		int commandSelectedFirst_{};
		int commandSelectedLast_{};

		bool hideRequested_{};
		bool wasCancelled_{}; // Set to true when the prompt was hidden manually by the user

		void saveReceiverCursor();
		void setPromptPosition();
		QPoint receiverCursorPosition();

		void centerViewOnPrompt() const;

		static const QString TYPE_HINT;
};

inline Visualization::Item* QueryPrompt::commandReceiver() { return commandReceiver_; }
//inline QString QueryPrompt::text() const {return command_->text();}
inline const std::unique_ptr<Visualization::Cursor>& QueryPrompt::commandReceiverCursor()
{ return commandReceiverCursor_;}

inline bool QueryPrompt::wasCancelled() const { return wasCancelled_; }
inline void QueryPrompt::cancelPrompt() {wasCancelled_ = true; hidePrompt();}

} /* namespace InformationScripting */
