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

#include "../interactionbase_api.h"

namespace Visualization {
	class Item;
	class StaticStyle;
}

namespace Interaction {

class CommandSuggestion;
class CommandResult;

class INTERACTIONBASE_API PromptMode
{
	public:
		virtual ~PromptMode();

		virtual Visualization::Item* createInputItem(const QString& initialCommandText) = 0;
		virtual Visualization::StaticStyle* modeIcon() const = 0;

		enum InputSelection
		{
			None,	/**< The input will not be selected. */
			All, /**< The entire input will be selected, so that it can be quickly deleted/replaced. */
			AtEnd /**< The input should have a cursor at the end, ready for additional input. */
		};
		virtual void setSelection(InputSelection selection) = 0;

		/**
		 * Called whenever the Prompt shell is about to be updated and repainted.
		 *
		 * The default implementation does nothing. Reimplement this in modes which need to update their state when
		 * the shell is updated. For example to provide auto-completion based on the latest input.
		 */
		virtual void onShellUpdate();

		/**
		 * Called when the handler of the prompt's shell detects and enter key press.
		 *
		 * The default implementation does nothing.
		 */
		virtual void onEnterKeyPress(Qt::KeyboardModifiers modifiers);

		/**
		 * Called when the handler of the prompt's shell detects a TAB key press.
		 *
		 * The default implementaiton does nothing.
		 */
		virtual void onTabKeyPress(Qt::KeyboardModifiers modifiers);

	protected:

		/**
		 * Call this method from derived classes in order to display a list of error item under the prompt.
		 *
		 * Ownership of the passed items will be transfered to the Prompt shell.
		 */
		void showErrors(QList<Visualization::Item*> errorItems) const;

		/**
		 * Call this method from derived classes in order to display a list of errors messages under the prompt.
		 */
		void showErrors(QList<QString> errorMessages) const;
};

}
