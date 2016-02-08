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

#include "EventRequestSet.h"

namespace OODebug {

StepData::StepData(qint64 threadId, Protocol::StepSize size, Protocol::StepDepth depth)
{
	threadID = threadId;
	stepSize = size;
	stepDepth = depth;
}

StepData::~StepData() {}

Modifier::~Modifier() {}

int Modifier::kind() const { return modKind(); }

Modifier Modifier::makeEventOff(qint32 count) {
	Modifier off{eventOff};
	off.count = count;
	return off;
}

Modifier Modifier::makeMatchClass(QString classPattern) {
	Modifier match{classMatch};
	match.classPattern = classPattern;
	return match;
}

Modifier Modifier::makeClassExclude(QString classPattern)
{
	Modifier exclude{classExclude};
	exclude.classExcludePatterm = classPattern;
	return exclude;
}

Modifier Modifier::makeLocation(Location loc)
{
	Modifier mod{locationOnly};
	mod.location = loc;
	return mod;
}

Modifier Modifier::makeSingleStep(qint64 threadId, Protocol::StepSize stepSize, Protocol::StepDepth stepDepth)
{
	Modifier mod{stepOnly};
	mod.step = StepData{threadId, stepSize, stepDepth};
	return mod;
}

Modifier::Modifier(int kind) {
	modKind = kind;
}

EventSetCommand::EventSetCommand(Protocol::EventKind kind)
	: Command{Protocol::CommandSet::EventRequest, Protocol::EventRequestCommands::Set}
{ this->kind = kind; }

EventSetCommand::~EventSetCommand() {}

BreakClassLoad::BreakClassLoad(QString classToBreak)
	: EventSetCommand{Protocol::EventKind::CLASS_PREPARE}
{
	suspendPolicy = Protocol::SuspendPolicy::ALL;
	modifiers = {Modifier::makeMatchClass(classToBreak), Modifier::makeEventOff(1)};
}

BreakClassLoad::~BreakClassLoad() {}


BreakpointCommand::BreakpointCommand(Location breakLocation) : EventSetCommand{Protocol::EventKind::BREAKPOINT}
{
	suspendPolicy = Protocol::SuspendPolicy::ALL;
	modifiers = {Modifier::makeLocation(breakLocation)};
}

BreakpointCommand::~BreakpointCommand() {}

StepCommand::StepCommand(qint64 threadId, Protocol::StepSize stepSize, Protocol::StepDepth stepDepth)
	: EventSetCommand{Protocol::EventKind::SINGLE_STEP}
{
	suspendPolicy = Protocol::SuspendPolicy::ALL;
	modifiers = {Modifier::makeSingleStep(threadId, stepSize, stepDepth), Modifier::makeClassExclude("java.*"),
					 Modifier::makeClassExclude("javax.*"), Modifier::makeClassExclude("sun.*"),
					 Modifier::makeClassExclude("com.sun.*"), Modifier::makeEventOff(1)};
}

StepCommand::~StepCommand() {}

EventSetReply::~EventSetReply() {}

EventClearCommand::EventClearCommand(Protocol::EventKind kind, qint32 requestId)
	: Command{Protocol::CommandSet::EventRequest, Protocol::EventRequestCommands::Clear}
{
	eventKind = kind;
	requestID = requestId;
}

EventClearCommand::~EventClearCommand() {}

}
