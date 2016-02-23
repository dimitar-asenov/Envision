/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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
#include "../DataTypes.h"
#include "../MessagePart.h"
#include "../Reply.h"

namespace OODebug {

struct OODEBUG_API LengthCommand : public Command {
		LengthCommand(qint64 arrayId);
		virtual ~LengthCommand() override;

		MessageField<qint64> arrayID{&LengthCommand::arrayID, this};
};

struct OODEBUG_API Length : public Reply {
		virtual ~Length() override;

		MessageField<qint32> arrayLength{&Length::arrayLength, this};
};

struct OODEBUG_API GetArrayValuesCommand : public Command {
		GetArrayValuesCommand(qint64 arrayId, qint32 firstIndex, qint32 length);
		virtual ~GetArrayValuesCommand() override;

		/* The array object ID. */
		MessageField<qint64> arrayObject{&GetArrayValuesCommand::arrayObject, this};
		/* The first index to retrieve. */
		MessageField<qint32> firstIndex{&GetArrayValuesCommand::firstIndex, this};
		/* The number of components to retrieve. */
		MessageField<qint32> length{&GetArrayValuesCommand::length, this};
};

struct OODEBUG_API ArrayValues : public Reply {
		virtual ~ArrayValues() override;

		/* The retrieved values. If the values are objects, they are tagged-values; otherwise, they are untagged-values */
		MessageField<Protocol::Tag> type{&ArrayValues::type, this};
		MessageField<QList<qint8>, cast(Protocol::Tag::BYTE)> bytes{&ArrayValues::bytes, this};
		MessageField<QList<qint16>, cast(Protocol::Tag::CHAR)> chars{&ArrayValues::chars, this};
		MessageField<QList<float>, cast(Protocol::Tag::FLOAT)> floats{&ArrayValues::floats, this};
		MessageField<QList<double>, cast(Protocol::Tag::DOUBLE)> doubles{&ArrayValues::doubles, this};
		MessageField<QList<qint32>, cast(Protocol::Tag::INT)> ints{&ArrayValues::ints, this};
		MessageField<QList<qint64>, cast(Protocol::Tag::LONG)> longs{&ArrayValues::longs, this};
		MessageField<QList<qint16>, cast(Protocol::Tag::SHORT)> shorts{&ArrayValues::shorts, this};
		MessageField<QList<qint8>, cast(Protocol::Tag::BOOLEAN)> booleans{&ArrayValues::booleans, this};

		MessageField<QList<Value>> values{&ArrayValues::values, this};

		virtual int kind() const override;
};

}
