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

#include "MessageBase.h"

namespace OODebug {

template <class T>
class MessageField
{
	public:
		inline MessageField(MessageBase* containingMessage, MessageBase::Direction dir = MessageBase::In) {
			if (dir & MessageBase::In)
			{
				containingMessage->addReadOperator([=] (QDataStream& stream)
				{
					stream >> value_;
				});
			}
			if (dir & MessageBase::Out)
			{
				containingMessage->addWriteOperator([=] (QDataStream& stream)
				{
					stream << value_;
				});
			}
		}

		inline T operator()() const { return value_; }
		inline T operator=(const T rhs) { return value_ = rhs; }

	private:
		T value_{};
};

QDataStream& operator>>(QDataStream& stream, QString& read)
{
	qint32 len;
	stream >> len;
	std::vector<char> data(len + 1);
	stream.readRawData(&data[0], len);
	data[len] = '\0';
	read = QString(&data[0]);
	return stream;
}

QDataStream& operator<<(QDataStream& stream, QString& write)
{
	// We know that value is of type QString so this is fine.
	qint32 len = write.length();
	// ignore the null terminator
	len = len - 1;
	QByteArray data = write.toLocal8Bit();
	stream.writeBytes(data.constData(), len);
	return stream;
}

template <class T>
typename std::enable_if<std::is_enum<T>::value, QDataStream&>::type
operator>>(QDataStream& stream, T& val)
{
	using Type = typename std::underlying_type<T>::type;
	Type rawValue;
	stream >> rawValue;
	val = static_cast<T>(rawValue);
	return stream;
}

template <class T>
typename std::enable_if<std::is_enum<T>::value, QDataStream&>::type
operator<<(QDataStream& stream, T& val)
{
	using Type = typename std::underlying_type<T>::type;
	Type rawValue = static_cast<Type>(val);
	return stream << rawValue;
}

} /* namespace OODebug */
