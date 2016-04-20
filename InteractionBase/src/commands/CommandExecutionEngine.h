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

#include "../interactionbase_api.h"

#include "VisualizationBase/src/items/Item.h"

namespace Interaction {

class CommandResult;
class CommandSuggestion;
class GenericHandler;

class INTERACTIONBASE_API CommandExecutionEngine
{
	public:
		virtual ~CommandExecutionEngine();
		static CommandExecutionEngine* instance();

		virtual void execute(Visualization::Item* originator, const QString& command,
				const std::unique_ptr<Visualization::Cursor>& cursor);
		virtual QList<CommandSuggestion*> autoComplete(Visualization::Item* originator, const QString& textSoFar,
				const std::unique_ptr<Visualization::Cursor>& cursor);

		QSharedPointer<CommandResult> result();

	protected:
		CommandExecutionEngine() = default;

		virtual QString extractNavigationString(QString& command);
		virtual Visualization::Item* navigate(Visualization::Item *originator, const QString& navigationString);

		virtual QStringList tokenize(const QString&string, const QString& quoteSymbols, const QString& escapeSymbols);
		virtual QStringList tokenizeNonQuoted(const QString&string);
		virtual bool doQuotesMatch(const QString& string, const QString& quoteSymbols, const QString& escapeSymbols);
		virtual bool isEscaped(const QString& string, int indexToCheck, const QString& escapeSymbols);

	private:
		QSharedPointer<CommandResult> lastCommandResult_;

		QList<CommandSuggestion*> suggestionsForHandler(GenericHandler* handler, QSet<std::size_t>& alreadySuggested,
																		QString commandText, Visualization::Item* source,
																		Visualization::Item* target,
																		const std::unique_ptr<Visualization::Cursor>& cursor);
};

inline QSharedPointer<CommandResult> CommandExecutionEngine::result() { return lastCommandResult_; }

}
