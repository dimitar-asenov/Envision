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

namespace OODebug {

// https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_ThreadReference

struct OODEBUG_API ThreadNameCommand : public Command {
		ThreadNameCommand(qint64 threadId);
		virtual ~ThreadNameCommand() override;

		MessageField<qint64> threadID{&ThreadNameCommand::threadID, this};
};

struct OODEBUG_API ThreadName : public Reply {
		virtual ~ThreadName() override;

		MessageField<QString> threadName{&ThreadName::threadName, this};
};

struct OODEBUG_API FramesCommand : public Command {
		FramesCommand(qint64 threadId, qint32 startFrame, qint32 numberOfFrames);
		virtual ~FramesCommand() override;
		/** The thread object ID.  */
		MessageField<qint64> thread{&FramesCommand::thread, this};
		/** The index of the first frame to retrieve. */
		MessageField<qint32> startFrame{&FramesCommand::startFrame, this};
		/** The count of frames to retrieve (-1 means all remaining).  */
		MessageField<qint32> length{&FramesCommand::length, this};
};

struct OODEBUG_API Frame : public MessagePart {
		virtual ~Frame() override;
		/** The ID of this frame.  */
		MessageField<qint64> frameID{&Frame::frameID, this};
		/** The current location of this frame */
		MessageField<Location> location{&Frame::location, this};
};

struct OODEBUG_API Frames : public Reply {
		virtual ~Frames() override;
		MessageField<QList<Frame>> frames{&Frames::frames, this};
};

}
