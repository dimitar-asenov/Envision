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

/*! \file
 * Describes datatypes from JDWP, which are relevant for us. You can find the spec under "Detailed Command Information":
 * https://docs.oracle.com/javase/7/docs/technotes/guides/jpda/jdwp-spec.htmls
 */

namespace OODebug {

struct OODEBUG_API Location : public MessagePart
{
		Location() = default;
		Location(Protocol::TypeTagKind typeTag, qint64 classId, qint64 methodId, qint64 methodIndex);
		virtual ~Location() override;
		MessageField<Protocol::TypeTagKind> typeTag{&Location::typeTag, this};
		MessageField<qint64> classId{&Location::classId, this};
		MessageField<qint64> methodId{&Location::methodId, this};
		MessageField<qint64> methodIndex{&Location::methodIndex, this};
		bool operator==(const Location& other) const;
};

uint qHash(const Location& location);

struct OODEBUG_API Value : public MessagePart {
		virtual ~Value() override;
		MessageField<Protocol::Tag> type{&Value::type, this};
		MessageField<qint64, cast(Protocol::Tag::ARRAY)> array{&Value::array, this};
		MessageField<qint8, cast(Protocol::Tag::BYTE)> byte{&Value::byte, this};
		MessageField<qint16, cast(Protocol::Tag::CHAR)> charValue{&Value::charValue, this};
		MessageField<qint64, cast(Protocol::Tag::OBJECT)> objectId{&Value::objectId, this};
		MessageField<float, cast(Protocol::Tag::FLOAT)> floatValue{&Value::floatValue, this};
		MessageField<double, cast(Protocol::Tag::DOUBLE)> doubleValue{&Value::doubleValue, this};
		MessageField<qint32, cast(Protocol::Tag::INT)> intValue{&Value::intValue, this};
		MessageField<qint64, cast(Protocol::Tag::LONG)> longValue{&Value::longValue, this};
		MessageField<qint16, cast(Protocol::Tag::SHORT)> shortValue{&Value::shortValue, this};
		MessageField<qint8, cast(Protocol::Tag::BOOLEAN)> boolean{&Value::boolean, this};
		MessageField<qint64, cast(Protocol::Tag::STRING)> stringId{&Value::stringId, this};

		virtual int kind() const override;
};

struct OODEBUG_API TaggedObjectId : public MessagePart {
		virtual ~TaggedObjectId() override;

		MessageField<Protocol::Tag> type{&TaggedObjectId::type, this};
		MessageField<qint64> objectId{&TaggedObjectId::objectId, this};
};

}
