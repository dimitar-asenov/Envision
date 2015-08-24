/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/
#include "KeyInputHandler.h"

namespace Interaction {

KeyInputHandler* KeyInputHandler::instance()
{
	static KeyInputHandler instance;
	return &instance;
}

void KeyInputHandler::registerInputHandler(const QString &eventName, QKeySequence keys,
										   InputState state, const InputHandler handler)
{
	registerInputHandler(eventName, QList<QKeySequence>({keys}), state, handler);
}

void KeyInputHandler::registerInputHandler(const QString &eventName, QList<QKeySequence> keys,
										   InputState state, const InputHandler handler)
{
	handlers_.append(RegisteredHandler{eventName, keys, state, handler});
}

void KeyInputHandler::handleKeyInput(Visualization::Item* target, QKeySequence keys)
{
	bool foundMatch = false;
	//If we find an exact match, execute that
	for (auto handler : handlers_)
		for (auto key : handler.keys_)
			if (key.matches(keys) == QKeySequence::ExactMatch
				&& (handler.state_ == state() || handler.state_ == AnyState))
			{
				handler.handler_(target, keys, state());
				foundMatch = true;
			}
	//In case we didn't find an exact match, see if the input is a superset
	//of an existing registered event
	if (!foundMatch)
		for (auto handler : handlers_)
			for (auto key : handler.keys_)
			{
				auto pattern = QRegExp("*" + key.toString() + "*",
							Qt::CaseInsensitive, QRegExp::Wildcard);
				if (pattern.exactMatch(keys.toString())
					&& (handler.state_ == state() || handler.state_ == AnyState))
				{
					handler.handler_(target, keys, state());
					foundMatch = true;
				}
			}
	//If no match, use the default handler
	if (!foundMatch && defaultHandler_)
		defaultHandler_(target, keys, state());
}

}
