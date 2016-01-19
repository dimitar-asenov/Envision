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

class PromptShell;
class PromptMode;

class INTERACTIONBASE_API Prompt
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

		template <typename ModeType>
		static void registerMode(const QString& modeName);

		using PromptShowCall = std::function<void (Visualization::Item*)>;
		static void registerPromptShowShortcut(Qt::Key key, PromptShowCall showFunction);
		static PromptShowCall showShortcut(Qt::Key key);

		static const QString& defaultModeName();

		static PromptMode* mode();
		static PromptShell* shell();

		static Visualization::Item* commandReceiver();

		// TODO: change this to return a plain pointer type
		static const std::unique_ptr<Visualization::Cursor>& commandReceiverCursor();
		static QPoint commandReceiverCursorPosition();

	private:
		static PromptShell* shell_;
		static PromptMode* mode_;

		static Visualization::Item* commandReceiver_;
		static std::unique_ptr<Visualization::Cursor> commandReceiverCursor_;

		using ModeConstructor = std::function<PromptMode* ()>;
		static QMap<QString, ModeConstructor>& modeRegistry();

		static QMap<Qt::Key, PromptShowCall>& promptShowShortcuts();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Prompt::PromptOptions)

inline bool Prompt::isVisible() { return mode_; }
inline PromptMode* Prompt::mode() { return mode_; }
inline PromptShell* Prompt::shell() { return shell_; }
inline Visualization::Item* Prompt::commandReceiver() { return commandReceiver_; }
inline const std::unique_ptr<Visualization::Cursor>& Prompt::commandReceiverCursor()
{ return commandReceiverCursor_; }
inline QPoint Prompt::commandReceiverCursorPosition()
	{ return commandReceiverCursor_ ? commandReceiverCursor_->position() : QPoint{0, 0}; }

template <typename ModeType>
inline void Prompt::registerMode(const QString& modeName)
{
	Q_ASSERT(!modeRegistry().contains(modeName));
	modeRegistry().insert(modeName, []()->PromptMode* {return new ModeType{};});
}

}
