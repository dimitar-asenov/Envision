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
#include "../Location.h"
#include "../MessagePart.h"
#include "../Reply.h"

// https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_EventRequest_Set

namespace OODebug {

class Modifier : public MessagePart
{
	public:
		static const int eventOff = 1;
		static const int classMatch = 5;
		static const int locationOnly = 7;

		static Modifier makeEventOff(qint32 count);
		static Modifier makeMatchClass(QString classPattern);
		static Modifier makeLocation(Location loc);

		Modifier() = default; // needed for in stream
		virtual ~Modifier() override;

		MessageField<qint8> modKind{&Modifier::modKind, this};
		MessageField<qint32, eventOff> count{&Modifier::count, this};
		MessageField<QString, classMatch> classPattern{&Modifier::classPattern, this};
		MessageField<Location, locationOnly> location{&Modifier::location, this};

		virtual int kind() const override;
	private:
		Modifier(int kind);
};

struct EventSetCommand : public Command {
		virtual ~EventSetCommand() override;
		MessageField<Protocol::EventKind> kind{&EventSetCommand::kind, this};
		MessageField<Protocol::SuspendPolicy> suspendPolicy{&EventSetCommand::suspendPolicy, this};

		MessageField<QList<Modifier>> modifiers{&EventSetCommand::modifiers, this};

	protected:
		EventSetCommand(Protocol::EventKind kind);
};

struct BreakClassLoad : public EventSetCommand
{
		BreakClassLoad(QString classToBreak);
		virtual ~BreakClassLoad() override;
};

struct BreakpointCommand : public EventSetCommand {
		BreakpointCommand(Location breakLocation);
		virtual ~BreakpointCommand() override;
};

struct EventSetReply : public Reply {
		virtual ~EventSetReply() override;
		MessageField<qint32> requestId{&EventSetReply::requestId, this};
};

} /* namespace OODebug */
