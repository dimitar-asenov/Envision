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

#include "Prompt.h"
#include "PromptShell.h"
#include "PromptMode.h"
#include "../autocomplete/AutoComplete.h"
#include "VisualizationBase/src/VisualizationManager.h"

namespace Interaction {

PromptShell* Prompt::shell_{};
Visualization::Item* Prompt::commandReceiver_{};
PromptMode* Prompt::mode_{};
std::unique_ptr<Visualization::Cursor> Prompt::commandReceiverCursor_{};

QMap<QString, Prompt::ModeConstructor>& Prompt::modeRegistry()
{
	static QMap<QString, ModeConstructor> map;
	return map;
}

QMap<Qt::Key, Prompt::PromptShowCall>& Prompt::promptShowShortcuts()
{
	static QMap<Qt::Key, PromptShowCall> map;
	return map;
}

void Prompt::registerPromptShowShortcut(Qt::Key key, PromptShowCall showFunction)
{
	Q_ASSERT(key != Qt::Key_Escape);
	Q_ASSERT(!promptShowShortcuts().contains(key));
	Q_ASSERT(showFunction);
	promptShowShortcuts().insert(key, showFunction);
}

Prompt::PromptShowCall Prompt::showShortcut(Qt::Key key)
{
	auto shortcut = promptShowShortcuts().find(key);
	if (shortcut == promptShowShortcuts().end()) return {};
	else return *shortcut;
}

const QString& Prompt::defaultModeName()
{
	static QString def{"command"};
	return def;
}

void Prompt::show(Visualization::Item* commandReceiver, QString initialCommandText, PromptOptions options)
{
	show({}, commandReceiver, initialCommandText, options );
}

void Prompt::show(const QString& modeName, Visualization::Item* commandReceiver, QString initialCommandText,
									PromptOptions options)
{
	Q_ASSERT(!shell_ && !mode_);

	if (options.testFlag(AutoHint) && initialCommandText.isEmpty())
		options |= InputHasHint;

	commandReceiver_ = commandReceiver;

	if (commandReceiver_->scene()->mainCursor() && commandReceiver_->scene()->mainCursor()->owner() == commandReceiver_)
		commandReceiverCursor_.reset(commandReceiver_->scene()->mainCursor()->clone());

	auto modeEntryIt = modeRegistry().find(modeName.isNull() ? defaultModeName() : modeName);
	Q_ASSERT(modeEntryIt != modeRegistry().end());
	mode_ = (*modeEntryIt)();
	Q_ASSERT(mode_);

	shell_ = new PromptShell{initialCommandText, options};
}

void Prompt::hide()
{
	if (isVisible())
	{
		Q_ASSERT(shell_);

		// Delete these two later. We need to copy them so that we still remember the current value.
		auto shellCopy = shell_;
		auto modeCopy = mode_;
		auto scene = Visualization::VisualizationManager::instance().mainScene();
		QApplication::postEvent(scene, new Visualization::CustomSceneEvent{[shellCopy, modeCopy](){
			delete shellCopy;
			delete modeCopy;
		}});

		// Select the item that was selected previously
		if (scene->mainCursor()) // If the main cursor was deleted, then do not select anything.
			commandReceiver_->moveCursor(Visualization::Item::MoveOnPosition, commandReceiverCursorPosition());

		shell_->removeFromScene();
		shell_ = nullptr;
		mode_ = nullptr;
		commandReceiver_ = nullptr;
		commandReceiverCursor_.reset();

		AutoComplete::hide();
	}
}

}
