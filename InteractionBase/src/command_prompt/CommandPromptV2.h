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
#include "VisualizationBase/src/cursor/Cursor.h"

namespace Interaction {

class CommandPromptShell;
class CommandPromptMode;

class INTERACTIONBASE_API CommandPromptV2
{
	public:

		enum PromptOption {
			None = 0,
			CenterViewOnPrompt = 0x00000001, /**< If set, the view will be scrolled so that the prompt is in the center. */
			InputHasHint = 0x00000002, /**< If set, the entire input will be selected, allowing the user to replace it
												  with a single keypress. This is useful if the input is a hint message. */
			AutoHint = 0x00000004, /**< If set, the hint option is automatically assumed if the initial text is not
												 empty. */
		};
		Q_DECLARE_FLAGS(PromptOptions, PromptOption)

		static void show(const QString& modeName, Visualization::Item* commandReceiver, QString initialCommandText = {},
							  PromptOptions options = AutoHint);
		static void show(Visualization::Item* commandReceiver, QString initialCommandText = {},
							  PromptOptions options = AutoHint);
		static void hide();
		static bool isVisible();

		template <class ModeType>
		static void registerMode(const QString& modeName);

		static const QString& defaultModeName();

		static CommandPromptMode* mode();
		static CommandPromptShell* shell();

		static Visualization::Item* commandReceiver();

		// TODO: change this to return a plain pointer type
		static const std::unique_ptr<Visualization::Cursor>& commandReceiverCursor();
		static QPoint commandReceiverCursorPosition();

	private:
		static CommandPromptShell* shell_;
		static CommandPromptMode* mode_;

		static Visualization::Item* commandReceiver_;
		static std::unique_ptr<Visualization::Cursor> commandReceiverCursor_;

		using ModeConstructor = std::function<CommandPromptMode* ()>;
		static QMap<QString, ModeConstructor>& modeRegistry();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(CommandPromptV2::PromptOptions)

inline bool CommandPromptV2::isVisible() { return mode_; }
inline CommandPromptMode* CommandPromptV2::mode() { return mode_; }
inline CommandPromptShell* CommandPromptV2::shell() { return shell_; }
inline Visualization::Item* CommandPromptV2::commandReceiver() { return commandReceiver_; }
inline const std::unique_ptr<Visualization::Cursor>& CommandPromptV2::commandReceiverCursor()
{ return commandReceiverCursor_; }
inline QPoint CommandPromptV2::commandReceiverCursorPosition()
	{ return commandReceiverCursor_ ? commandReceiverCursor_->position() : QPoint(0, 0); }

template <class ModeType>
inline void CommandPromptV2::registerMode(const QString& modeName)
{
	Q_ASSERT(!modeRegistry().contains(modeName));
	modeRegistry().insert(modeName, []()->CommandPromptMode* {return new ModeType();});
}

}
