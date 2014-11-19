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

#include "../../../oodebug_api.h"

#include "../Command.h"
#include "../MessagePart.h"
#include "../Reply.h"

// https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_EventRequest_Set

namespace OODebug {

class Modifier : public MessagePart
{
	public:
		static const int eventOff = 1;
		static const int classMatch = 5;

		Modifier() {} // needed for in stream
		MessageField<qint8> modKind{&Modifier::modKind, this};
		MessageField<qint32, eventOff> count{&Modifier::count, this};
		MessageField<QString, classMatch> classPattern{&Modifier::classPattern, this};

		virtual int kind() const override;

		static Modifier makeEventOff(qint32 count) {
			Modifier off(eventOff);
			off.count = count;
			return off;
		}

		static Modifier makeMatchClass(QString classPattern) {
			Modifier match(classMatch);
			match.classPattern = classPattern;
			return match;
		}

	private:
		Modifier(int kind) {
			modKind = kind;
		}
};

int Modifier::kind() const { return modKind(); }

struct EventSetCommand : public Command {
		template <class T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
		EventSetCommand(int id, T command) : Command(id, Protocol::CommandSet::EventRequest, command) {}

		MessageField<Protocol::EventKind> kind{&EventSetCommand::kind, this};
		MessageField<Protocol::SuspendPolicy> suspendPolicy{&EventSetCommand::suspendPolicy, this};

		MessageField<QList<Modifier>> modifiers{&EventSetCommand::modifiers, this};
};

struct BreakClassLoad : public EventSetCommand
{
		BreakClassLoad(int id, QString classToBreak) : EventSetCommand(id, Protocol::EventRequestCommands::Set) {
			kind = Protocol::EventKind::CLASS_PREPARE;
			suspendPolicy = Protocol::SuspendPolicy::ALL;
			modifiers = {Modifier::makeMatchClass(classToBreak), Modifier::makeEventOff(1)};
		}
};

struct EventSetReply : public Reply {
		MessageField<qint32> requestId{&EventSetReply::requestId, this};
};

} /* namespace OODebug */
