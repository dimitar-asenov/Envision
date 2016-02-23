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
#include "../Reply.h"
#include "../Protocol.h"

// All needed messages from: https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_Method

namespace OODebug {

struct OODEBUG_API LineTableCommand : public Command {
		LineTableCommand(qint64 classId, qint64 methodId);
		virtual ~LineTableCommand() override;
		MessageField<qint64> refTypeId{&LineTableCommand::refTypeId, this};
		MessageField<qint64> methodID{&LineTableCommand::methodID, this};
};

struct OODEBUG_API CodeIndexLine : public MessagePart {
		virtual ~CodeIndexLine() override;
		MessageField<qint64> lineCodeIndex{&CodeIndexLine::lineCodeIndex, this};
		MessageField<qint32> lineNumber{&CodeIndexLine::lineNumber, this};
};

struct OODEBUG_API LineTable : public Reply {
		virtual ~LineTable() override;
		/** Lowest valid code index for the method, >=0, or -1 if the method is native */
		MessageField<qint64> start{&LineTable::start, this};
		/** Highest valid code index for the method, >=0, or -1 if the method is native */
		MessageField<qint64> end{&LineTable::end, this};
		MessageField<QList<CodeIndexLine>> mappings{&LineTable::mappings, this};
};

struct OODEBUG_API VariableTableCommand : public Command {
		VariableTableCommand(qint64 classId, qint64 methodId);
		virtual ~VariableTableCommand() override;
		MessageField<qint64> refTypeId{&VariableTableCommand::refTypeId, this};
		MessageField<qint64> methodID{&VariableTableCommand::methodID, this};
};

struct OODEBUG_API VariableDetails : public MessagePart {
		virtual ~VariableDetails() override;
		/** First code index at which the variable is visible (unsigned).
		 * Used in conjunction with length. The variable can be get or set only when:
		 *  the current codeIndex <= current frame code index < codeIndex + length */
		MessageField<qint64> codeIndex{&VariableDetails::codeIndex, this};
		/** The variable's name. */
		MessageField<QString> name{&VariableDetails::name, this};
		/** The variable type's JNI signature. */
		MessageField<QString> signature{&VariableDetails::signature, this};
		/** Unsigned value used in conjunction with codeIndex. The variable can be get or set only when
		 *  the current codeIndex <= current frame code index < code index + length */
		MessageField<qint32> length{&VariableDetails::length, this};
		/** The local variable's index in its frame */
		MessageField<qint32> slot{&VariableDetails::slot, this};
};

struct OODEBUG_API VariableTable : public Reply {
		virtual ~VariableTable() override;
		/** The number of words in the frame used by arguments. Eight-byte arguments use two words; all others use one. */
		MessageField<qint32> argCnt{&VariableTable::argCnt, this};
		/** The number of variables. */
		MessageField<QList<VariableDetails>> variables{&VariableTable::variables, this};
};

}
