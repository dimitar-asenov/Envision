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
#include "Message.h"

namespace OODebug {

template <class T, class >
class MessageItem
{
	public:
		enum class Direction {In = 0x1, Out = 0x2, InOut = 0x3};

		inline MessageItem(Message* containingMessage, Direction dir = Direction::In) {
			if (dir & Direction::In)
			{
				containingMessage->addReadOperator([=] (QDataStream& stream)
				{
					stream >> value_;
				});
			}
			if (dir & Direction::Out)
			{
				containingMessage->addWriteOperator([=] (QDataStream& stream)
				{
					stream << value_;
				});
			}
		}

		inline const T& operator()() const { return value_; }
		inline T& operator=(const T& rhs) { return value_ = T(rhs); }

	private:
		T value_{};
};

template <class T>
class MessageItem<T, typename std::enable_if<std::is_same<T, QString>::value>::type>
{
	public:
		inline MessageItem(Message* containingMessage, Direction dir = Direction::In) {
			if (dir & Direction::In)
			{
				containingMessage->addReadOperator([=] (QDataStream& stream)
				{
					qint32 len;
					stream >> len;
					char data[len + 1];
					stream.readRawData(data, len);
					data[len] = '\0';
					value_ = QString(data);
				});
			}
			if (dir & Direction::Out)
			{
				containingMessage->addWriteOperator([=] (QDataStream& stream)
				{
					// We know that value is of type QString so this is fine.
					qint32 len = value_.length();
					// ignore the null terminator
					len = len - 1;
					QByteArray data = value_.toLocal8Bit();
					stream.writeBytes(data.constData(), len);
				});
			}
		}

		inline const T& operator()() const { return value_; }
		inline T& operator=(const T& rhs) { return value_ = T(rhs); }

	private:
		T value_{};
};

template <class T>
class MessageItem<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
	public:
		inline MessageItem(Message* containingMessage, Direction dir = Direction::In) {
			if (dir & Direction::In)
			{
				containingMessage->addReadOperator([=] (QDataStream& stream)
				{
					Type rawValue;
					stream >> rawValue;
					value_ = static_cast<T>(rawValue);
				});
			}
			if (dir & Direction::Out)
			{
				containingMessage->addWriteOperator([=] (QDataStream& stream)
				{
					Type rawValue = static_cast<Type>(value_);
					stream << rawValue;
				});
			}
		}

		inline T operator()() const { return value_; }
		inline T& operator=(const T& rhs) { return value_ = T(rhs); }

	private:
		T value_{};
		using Type = typename std::underlying_type<T>::type;
};

} /* namespace OODebug */
