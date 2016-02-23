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
#include "../DataTypes.h"

// https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_Event_Composite

namespace OODebug {

struct OODEBUG_API VMStart : public MessagePart
{
		virtual ~VMStart() override;
		MessageField<qint32> requestId{&VMStart::requestId, this};
		MessageField<qint64> threadId{&VMStart::threadId, this};
};

struct OODEBUG_API SingleStepEvent : public MessagePart
{
		virtual ~SingleStepEvent() override;
		MessageField<qint32> requestID{&SingleStepEvent::requestID, this};
		MessageField<qint64> thread{&SingleStepEvent::thread, this};
		MessageField<Location> location{&SingleStepEvent::location, this};
};

struct OODEBUG_API ClassPrepare : public MessagePart
{
		virtual ~ClassPrepare() override;
		MessageField<qint32> requestId{&ClassPrepare::requestId, this};
		MessageField<qint64> threadId{&ClassPrepare::threadId, this};
		MessageField<Protocol::TypeTagKind> refTypeTag{&ClassPrepare::refTypeTag, this};
		MessageField<qint64> typeID{&ClassPrepare::typeID, this};
		MessageField<QString> signature{&ClassPrepare::signature, this};
		MessageField<qint32> status{&ClassPrepare::status, this};
};

struct OODEBUG_API BreakpointEvent : public MessagePart
{
		virtual ~BreakpointEvent() override;
		MessageField<qint32> requestID{&BreakpointEvent::requestID, this};
		MessageField<qint64> thread{&BreakpointEvent::thread, this};
		MessageField<Location> location{&BreakpointEvent::location, this};
};

struct OODEBUG_API Event : public MessagePart
{
		virtual ~Event() override;
		MessageField<Protocol::EventKind> eventKind{&Event::eventKind, this};

		MessageField<VMStart, cast(Protocol::EventKind::VM_START)> vmStart{&Event::vmStart, this};
		MessageField<SingleStepEvent, cast(Protocol::EventKind::SINGLE_STEP)> singleStep{&Event::singleStep, this};
		MessageField<BreakpointEvent, cast(Protocol::EventKind::BREAKPOINT)> breakpoint{&Event::breakpoint, this};
		MessageField<ClassPrepare, cast(Protocol::EventKind::CLASS_PREPARE)> classPrepare{&Event::classPrepare, this};

		virtual int kind() const override;
};

struct OODEBUG_API CompositeCommand : public Command {
		virtual ~CompositeCommand() override;
		MessageField<Protocol::SuspendPolicy> suspendPolicy{&CompositeCommand::suspendPolicy, this};
		MessageField<QList<Event>> events{&CompositeCommand::events, this};
};


}
