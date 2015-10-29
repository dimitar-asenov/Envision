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

#include "CommandPromptV2.h"
#include "CommandPromptShell.h"
#include "CommandPromptMode.h"

namespace Interaction {

CommandPromptShell* CommandPromptV2::shell_{};
Visualization::Item* CommandPromptV2::commandReceiver_{};
CommandPromptMode* CommandPromptV2::mode_{};
std::unique_ptr<Visualization::Cursor> CommandPromptV2::commandReceiverCursor_{};

QMap<QString, QPair<QString, CommandPromptV2::ModeConstructor>>& CommandPromptV2::modeRegistry()
{
	static QMap<QString, QPair<QString, ModeConstructor>> map;
	return map;
}

const QString& CommandPromptV2::defaultModeName()
{
	static QString def{"command"};
	return def;
}

void CommandPromptV2::show(Visualization::Item* commandReceiver, QString initialCommandText, PromptOptions options)
{
	show({}, commandReceiver, initialCommandText, options );
}

void CommandPromptV2::show(const QString& modeName, Visualization::Item* commandReceiver, QString initialCommandText,
									PromptOptions options)
{
	Q_ASSERT(!shell_ && !mode_);

	commandReceiver_ = commandReceiver;

	if (commandReceiver_->scene()->mainCursor() && commandReceiver_->scene()->mainCursor()->owner() == commandReceiver_)
		commandReceiverCursor_.reset(commandReceiver_->scene()->mainCursor()->clone());

	auto modeEntryIt = modeRegistry().find(modeName.isNull() ? defaultModeName() : modeName);
	Q_ASSERT(modeEntryIt != modeRegistry().end());
	mode_ = modeEntryIt->second();
	Q_ASSERT(mode_);

	shell_ = new CommandPromptShell(mode_->createInputItem(initialCommandText), modeEntryIt->first, options);
}

void CommandPromptV2::hide()
{
	if (shell_)
	{
		SAFE_DELETE_ITEM(shell_);
		SAFE_DELETE(mode_);

		commandReceiver_ = nullptr;
		commandReceiverCursor_.reset();
	}
}

}
