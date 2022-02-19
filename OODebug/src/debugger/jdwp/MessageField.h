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

#include "MessagePart.h"

namespace OODebug {

class Reply;
class Command;

template <typename T>
inline void read(QDataStream& stream, T& val)
{
	stream >> val;
}

template <typename T>
inline void write(QDataStream& stream, const T& val)
{
	stream << val;
}

// QDataStream uses Double precision per default,
// but the VM only sends us single precision floats thus we need to specialize the following four functions:
template <>
inline void read(QDataStream& stream, float& val)
{
	stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	stream >> val;
	stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
}

template <>
inline void write(QDataStream& stream, const float& val)
{
	stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	stream << val;
	stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
}

template <>
inline void read(QDataStream& stream, QList<float>& val)
{
	stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	stream >> val;
	stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
}

template <>
inline void write(QDataStream& stream, const QList<float>& val)
{
	stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	stream << val;
	stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
}

template <>
inline void read(QDataStream& stream, QString& read)
{
	qint32 len;
	stream >> len;
	std::vector<char> data(len + 1);
	stream.readRawData(&data[0], len);
	data[len] = '\0';
	read = QString{&data[0]};
}

template <>
inline void write(QDataStream& stream, const QString& write)
{
	// We know that value is of type QString so this is fine.
	qint32 len = write.length();
	QByteArray data = write.toLocal8Bit();
	stream.writeBytes(data.constData(), len);
}

template <typename T, int Kind = MessagePart::noKind>
class MessageField
{
	public:
		/**
		 * Creates a messagefield and adds the corresponding reader and writer functions to the \a containingMessage.
		 */
		template <typename Container>
		inline MessageField(MessageField<T, Kind> Container::*field, MessagePart* containingMessage);

		inline T operator()() const;
		inline T operator=(const T rhs);

	private:
		T value_{};
};

template <typename T, int Kind>
template <typename Container>
inline MessageField<T, Kind>::MessageField(MessageField<T, Kind> Container::*field, MessagePart* containingMessage) {
	auto reader = [field] (MessagePart* container, QDataStream& stream)
	{
		if (Kind == MessagePart::noKind || Kind == container->kind())
			read(stream, (static_cast<Container*>(container)->*field).value_);
	};
	auto writer = [field] (const MessagePart* container, QDataStream& stream)
	{
		if (Kind == MessagePart::noKind || Kind == container->kind())
			write(stream, (static_cast<const Container*>(container)->*field).value_);
	};
	containingMessage->addMessageField(reader, writer);
}

template <typename T, int Kind>
T MessageField<T, Kind>::operator()() const { return value_; }

template <typename T, int Kind>
T MessageField<T, Kind>::operator=(const T rhs) { return value_ = rhs; }

}
