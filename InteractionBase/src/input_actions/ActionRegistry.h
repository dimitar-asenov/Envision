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

#pragma once

#include "../interactionbase_api.h"

namespace Visualization {
	class Item;
}

namespace Interaction {

class ActionRegistry
{
	public:
		enum InputState {
			DefaultState,
			AnyState
		};

		static ActionRegistry* instance();

		using InputHandler = bool (*) (Visualization::Item* target, QKeySequence keys, InputState state);

		void registerInputHandler(const QString& eventName, const InputHandler handler);
		QStringList inputHandlers() const;

		bool handleKeyInput(Visualization::Item* target, QKeySequence keys, const QString& handlerName);

		void enterChangeShortcutState(const QString& eventName);

		void saveShortcuts();

		InputState state() const;
		void setState(InputState state);

	private:
		ActionRegistry();

		struct RegisteredHandler {
			QString eventName_;
			QList<QKeySequence> keys_;
			InputState state_;
			InputHandler handler_;
		};
		QList<RegisteredHandler*> handlers_;

		InputState state_{DefaultState};

		static const QHash<const QString, QKeySequence::StandardKey>& standardKeysMap();
};

inline ActionRegistry::InputState ActionRegistry::state() const { return state_; }
inline void ActionRegistry::setState(InputState state) { state_ = state; }

}
