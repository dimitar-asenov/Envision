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
#include "ActionRegistry.h"

#include "../InteractionBaseException.h"

namespace Interaction {

const QHash<const QString, QKeySequence::StandardKey>& ActionRegistry::standardKeysMap()
{
	static const QHash<const QString, QKeySequence::StandardKey> map {
		{"Standard-AddTab", QKeySequence::AddTab},
		{"Standard-Back", QKeySequence::Back},
		{"Standard-Bold", QKeySequence::Bold},
		{"Standard-Close", QKeySequence::Close},
		{"Standard-Copy", QKeySequence::Copy},
		{"Standard-Cut", QKeySequence::Cut},
		{"Standard-Delete", QKeySequence::Delete},
		{"Standard-DeleteEndOfLine", QKeySequence::DeleteEndOfLine},
		{"Standard-DeleteEndOfWord", QKeySequence::DeleteEndOfWord},
		{"Standard-DeleteStartOfWord", QKeySequence::DeleteStartOfWord},
		{"Standard-DeleteCompleteLine", QKeySequence::DeleteCompleteLine},
		{"Standard-Find", QKeySequence::Find},
		{"Standard-FindNext", QKeySequence::FindNext},
		{"Standard-FindPrevious", QKeySequence::FindPrevious},
		{"Standard-Forward", QKeySequence::Forward},
		{"Standard-HelpContents", QKeySequence::HelpContents},
		{"Standard-InsertLineSeparator", QKeySequence::InsertLineSeparator},
		{"Standard-InsertParagraphSeparator", QKeySequence::InsertParagraphSeparator},
		{"Standard-Italic", QKeySequence::Italic},
		{"Standard-MoveToEndOfBlock", QKeySequence::MoveToEndOfBlock},
		{"Standard-MoveToEndOfDocument", QKeySequence::MoveToEndOfDocument},
		{"Standard-MoveToEndOfLine", QKeySequence::MoveToEndOfLine},
		{"Standard-MoveToNextChar", QKeySequence::MoveToNextChar},
		{"Standard-MoveToNextLine", QKeySequence::MoveToNextLine},
		{"Standard-MoveToNextPage", QKeySequence::MoveToNextPage},
		{"Standard-MoveToNextWord", QKeySequence::MoveToNextWord},
		{"Standard-MoveToPreviousChar", QKeySequence::MoveToPreviousChar},
		{"Standard-MoveToPreviousLine", QKeySequence::MoveToPreviousLine},
		{"Standard-MoveToPreviousPage", QKeySequence::MoveToPreviousPage},
		{"Standard-MoveToPreviousWord", QKeySequence::MoveToPreviousWord},
		{"Standard-MoveToStartOfBlock", QKeySequence::MoveToStartOfBlock},
		{"Standard-MoveToStartOfDocument", QKeySequence::MoveToStartOfDocument},
		{"Standard-MoveToStartOfLine", QKeySequence::MoveToStartOfLine},
		{"Standard-New", QKeySequence::New},
		{"Standard-NextChild", QKeySequence::NextChild},
		{"Standard-Open", QKeySequence::Open},
		{"Standard-Paste", QKeySequence::Paste},
		{"Standard-Preferences", QKeySequence::Preferences},
		{"Standard-PreviousChild", QKeySequence::PreviousChild},
		{"Standard-Print", QKeySequence::Print},
		{"Standard-Quit", QKeySequence::Quit},
		{"Standard-Redo", QKeySequence::Redo},
		{"Standard-Refresh", QKeySequence::Refresh},
		{"Standard-Replace", QKeySequence::Replace},
		{"Standard-SaveAs", QKeySequence::SaveAs},
		{"Standard-Save", QKeySequence::Save},
		{"Standard-SelectAll", QKeySequence::SelectAll},
		{"Standard-Deselect", QKeySequence::Deselect},
		{"Standard-SelectEndOfBlock", QKeySequence::SelectEndOfBlock},
		{"Standard-SelectEndOfDocument", QKeySequence::SelectEndOfDocument},
		{"Standard-SelectEndOfLine", QKeySequence::SelectEndOfLine},
		{"Standard-SelectNextChar", QKeySequence::SelectNextChar},
		{"Standard-SelectNextLine", QKeySequence::SelectNextLine},
		{"Standard-SelectNextPage", QKeySequence::SelectNextPage},
		{"Standard-SelectNextWord", QKeySequence::SelectNextWord},
		{"Standard-SelectPreviousChar", QKeySequence::SelectPreviousChar},
		{"Standard-SelectPreviousLine", QKeySequence::SelectPreviousLine},
		{"Standard-SelectPreviousPage", QKeySequence::SelectPreviousPage},
		{"Standard-SelectPreviousWord", QKeySequence::SelectPreviousWord},
		{"Standard-SelectStartOfBlock", QKeySequence::SelectStartOfBlock},
		{"Standard-SelectStartOfDocument", QKeySequence::SelectStartOfDocument},
		{"Standard-SelectStartOfLine", QKeySequence::SelectStartOfLine},
		{"Standard-Underline", QKeySequence::Underline},
		{"Standard-Undo", QKeySequence::Undo},
		{"Standard-UnknownKey", QKeySequence::UnknownKey},
		{"Standard-WhatsThis", QKeySequence::WhatsThis},
		{"Standard-ZoomIn", QKeySequence::ZoomIn},
		{"Standard-ZoomOut", QKeySequence::ZoomOut},
		{"Standard-FullScreen", QKeySequence::FullScreen}
	};

	return map;
}

ActionRegistry* ActionRegistry::instance()
{
	static ActionRegistry instance;
	return &instance;
}

ActionRegistry::ActionRegistry()
{
	//Read all JSON keyboard binding files
	QDir bindingsDir{"key-bindings"};
	for (auto fileName : bindingsDir.entryList(QStringList{} << "*.json", QDir::Files))
	{
		QFile file{bindingsDir.absoluteFilePath(fileName)};
		file.open(QIODevice::ReadOnly);
		QTextStream read{&file};
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
			{
				// We only support strings
				if (shortcut.isString())
				{
					auto shortcutString = shortcut.toString();
					if (shortcutString.startsWith("Standard-"))
					{
						auto sequence = standardKeysMap().find(shortcutString);
						if (sequence != standardKeysMap().end())
							handler->keys_.append(QKeySequence{*sequence});
						else
							throw InteractionBaseException{"Unrecognized standard key sequence name: " + shortcutString};
					}
					else
					{
						handler->keys_.append(QKeySequence{shortcutString});
						if (handler->keys_.last().isEmpty())
							throw InteractionBaseException{"Unrecognized key: " + shortcutString};
					}
				}
				else throw InteractionBaseException{"Key names in " + fileName + " should be strings."};
			}

			handlers_.append(handler);
		}
	}
}

void ActionRegistry::registerInputHandler(const QString &eventName, const InputHandler handler)
{
	for (auto h : handlers_)
		if (h->eventName_ == eventName)
		{
			h->handler_ = handler;
			return;
		}

	Q_ASSERT(false && "Handler has no input keys defined in any key binding file");
}

bool ActionRegistry::handleKeyInput(Visualization::Item* target, QKeySequence keys, const QString& handlerName)
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
				auto pattern = QRegExp{"*" + key.toString() + "*",
							Qt::CaseInsensitive, QRegExp::Wildcard};
				if (pattern.exactMatch(keys.toString()) && handler->handler_
						&& (handler->state_ == state() || handler->state_ == AnyState))
					handled = handled || handler->handler_(target, keys, state());
			}

	return handled;
}

void ActionRegistry::saveShortcuts()
{
	QFile file{"inputs.json"};
	file.open(QIODevice::WriteOnly);
	QTextStream write{&file};

	//Store the JSON
	QJsonObject main;
	for (auto handler : handlers_)
	{
		QJsonObject info;
		info["state"] = (int) handler->state_;
		QJsonArray shortcuts;
		for (auto keys : handler->keys_)
			shortcuts.append(keys.toString());
		info["keys"] = shortcuts;
		main[handler->eventName_] = info;
	}
	write << (QJsonDocument{main}).toJson();
}

}
