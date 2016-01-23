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

KeyInputHandler::KeyInputHandler()
{
	//Read the JSON string
	QFile file("inputs.json");
	file.open(QIODevice::ReadOnly);
	QTextStream read(&file);
	QString json;
	while (!read.atEnd()) json = json + read.readLine();
	auto doc = QJsonDocument::fromJson(json.toUtf8());
	//Interpret the JSON, set the shortcuts for all possible events
	auto obj = doc.object();
	for (auto key : obj.keys())
	{
		QJsonObject current = obj[key].toObject();
		auto state = (InputState) current["state"].toInt();
		auto shortcuts = current["keys"].toArray();
		auto handler = new RegisteredHandler{key, {}, state, nullptr};
		for (auto shortcut : shortcuts)
			if (shortcut.isString())
				handler->keys_.append(QKeySequence(shortcut.toString()));
			else if (shortcut.isDouble())
				handler->keys_.append(QKeySequence((QKeySequence::StandardKey)shortcut.toInt()));
		handlers_.append(handler);
	}

	setDefaultHandler(changeShortcut, ChangeShortcutState);
}

void KeyInputHandler::registerInputHandler(const QString &eventName, const InputHandler handler)
{
	for (auto h : handlers_)
		if (h->eventName_ == eventName)
			h->handler_ = handler;
}

QStringList KeyInputHandler::inputHandlers() const
{
	QStringList result;
	for (auto handler : handlers_)
		result.append(handler->eventName_);
	return result;
}

void KeyInputHandler::setDefaultHandler(const InputHandler handler, InputState state)
{
	defaultHandlers_[state] = handler;
}

bool KeyInputHandler::handleKeyInput(Visualization::Item* target, QKeySequence keys, const QString& handlerName)
{
	bool handled = false;
	//If we find an exact match, execute that
	for (auto handler : handlers_)
		if (handler->eventName_.startsWith(handlerName))
			for (auto key : handler->keys_)
				if (key.matches(keys) == QKeySequence::ExactMatch && handler->handler_
						&& (handler->state_ == state() || handler->state_ == AnyState))
					handled = handled || handler->handler_(target, keys, state());

	//In case we didn't find an exact match, see if the input is a superset
	//of an existing registered event
	for (auto handler : handlers_)
		if (handler->eventName_.startsWith(handlerName))
			for (auto key : handler->keys_)
			{
				auto pattern = QRegExp("*" + key.toString() + "*",
							Qt::CaseInsensitive, QRegExp::Wildcard);
				if (pattern.exactMatch(keys.toString()) && handler->handler_
						&& (handler->state_ == state() || handler->state_ == AnyState))
					handled = handled || handler->handler_(target, keys, state());
			}

	//If no match, use the default handler
	if (!handled && defaultHandlers_[state()])
		handled = defaultHandlers_[state()](target, keys, state());
	return handled;
}

void KeyInputHandler::enterChangeShortcutState(const QString &eventName)
{
	qDebug() << eventName;
	state_ = ChangeShortcutState;
	shortcutToChange_ = eventName;
}

bool KeyInputHandler::changeShortcut(Visualization::Item *, QKeySequence keys, InputState)
{
	qDebug() << "Change to " << keys.toString();
	auto ih = instance();
	ih->state_ = DefaultState;
	for (auto handler : ih->handlers_)
		if (handler->eventName_ == ih->shortcutToChange_)
		{
			if (handler->keys_.size() == 0)	handler->keys_.append(keys);
			else handler->keys_[handler->keys_.size() - 1] = keys;
		}
	return true;
}

}
