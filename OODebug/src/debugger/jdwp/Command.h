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

#include "../../oodebug_api.h"

#include "MessagePart.h"
#include "MessageField.h"
#include "Protocol.h"


namespace OODebug {

class OODEBUG_API Command : public MessagePart {
	public:
		Command();
		virtual ~Command() override;

		// Message header data:
		MessageField<qint32> length{&Command::length, this};
		MessageField<qint32> id{&Command::id, this};
		MessageField<qint8> flags{&Command::flags, this};
		MessageField<Protocol::CommandSet> commandSet{&Command::commandSet, this};
		MessageField<qint8> command{&Command::command, this};

		static void resetIds();

	protected:
		template <class T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
		Command(Protocol::CommandSet cmdSet, T cmd);

	private:
		static int nextId_;
};

template <class T, typename>
Command::Command(Protocol::CommandSet cmdSet, T cmd)
{
	id = ++nextId_;
	commandSet = cmdSet;
	command = static_cast<typename std::underlying_type<T>::type>(cmd);
}

} /* namespace OODebug */
