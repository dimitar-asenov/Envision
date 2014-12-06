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
#include "../Location.h"

namespace OODebug {

// https://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_StackFrame

struct StackVariable : public MessagePart {
		StackVariable() = default;
		StackVariable(qint32 slot, Protocol::Tag type);
		virtual ~StackVariable() override;
		/** The local variable's index in the frame.  */
		MessageField<qint32> slot{&StackVariable::slot, this};
		/** A tag identifying the type of the variable */
		MessageField<Protocol::Tag> sigbyte{&StackVariable::sigbyte, this};
};

struct GetValuesCommand : public Command {
		GetValuesCommand(qint64 threadId, qint64 frameId, QList<StackVariable> variables);
		virtual ~GetValuesCommand() override;
		/** The frame's thread. */
		MessageField<qint64> thread{&GetValuesCommand::thread, this};
		/** The frame ID. */
		MessageField<qint64> frame{&GetValuesCommand::frame, this};
		/** The number of values to get. */
		MessageField<QList<StackVariable>> variables{&GetValuesCommand::variables, this};
};

struct Value : public MessagePart {
		virtual ~Value() override;
		MessageField<Protocol::Tag> type{&Value::type, this};
		MessageField<qint64, cast(Protocol::Tag::ARRAY)> array{&Value::array, this};
		MessageField<qint8, cast(Protocol::Tag::BYTE)> byte{&Value::byte, this};
		MessageField<qint16, cast(Protocol::Tag::CHAR)> charValue{&Value::charValue, this};
		MessageField<qint64, cast(Protocol::Tag::OBJECT)> objectId{&Value::objectId, this};
		MessageField<qint32, cast(Protocol::Tag::FLOAT)> floatValue{&Value::floatValue, this};
		MessageField<qint64, cast(Protocol::Tag::DOUBLE)> doubleValue{&Value::doubleValue, this};
		MessageField<qint32, cast(Protocol::Tag::INT)> intValue{&Value::intValue, this};
		MessageField<qint64, cast(Protocol::Tag::LONG)> longValue{&Value::longValue, this};
		MessageField<qint16, cast(Protocol::Tag::SHORT)> shortValue{&Value::shortValue, this};
		MessageField<qint8, cast(Protocol::Tag::BOOLEAN)> boolean{&Value::boolean, this};
		MessageField<qint64, cast(Protocol::Tag::STRING)> stringId{&Value::stringId, this};

		virtual int kind() const override;
};

struct Values : public Reply {
		MessageField<QList<Value>> values{&Values::values, this};
};

} /* namespace OODebug */
