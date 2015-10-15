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

#include "../informationscripting_api.h"

namespace InformationScripting {

/**
 * Class inspired by std::experimental::optional, but it additionally contains an error/warning message.
 */
template <class ValueType>
class INFORMATIONSCRIPTING_API Optional
{
		// Since the error constructor takes a QString we disallow usage with QString.
		static_assert(!std::is_same<ValueType, QString>(), "QString is not allowed as paremeter for Optional");
	public:
		constexpr Optional(const ValueType& v, const QString& warning = {});
		constexpr Optional(ValueType&& v, const QString& warning = {});
		constexpr Optional(const QString& errorMessage);
		constexpr Optional(QString&& errorMessage);

		Optional(const Optional& other);
		Optional(Optional&& other);

		constexpr explicit operator bool() const;

		constexpr const ValueType& value() const &;
		constexpr ValueType& value() &;

		constexpr ValueType&& value() &&;
		constexpr const ValueType&& value() const &&;

		constexpr bool hasError() const;
		QString error() const;

		constexpr bool hasWarning() const;
		QString warning() const;
	private:
		enum class Type : int {Value, Warning, Error};
		ValueType value_{};
		QString message_;
		Type type_{Type::Value};
};

template <class ValueType>
inline constexpr Optional<ValueType>::Optional(const ValueType& v, const QString& warning)
	: value_{v}, message_{warning}
{
	if (!warning.isNull()) type_ = Type::Warning;
}

template <class ValueType>
inline constexpr Optional<ValueType>::Optional(ValueType&& v, const QString& warning)
	: value_{std::move(v)}, message_{warning}
{
	if (!warning.isNull()) type_ = Type::Warning;
}

template <class ValueType>
inline constexpr Optional<ValueType>::Optional(const QString& errorMessage)
	: message_{errorMessage}, type_{Type::Error} {}

template <class ValueType>
inline constexpr Optional<ValueType>::Optional(QString&& errorMessage)
	: message_{std::move(errorMessage)}, type_{Type::Error} {}

template <class ValueType>
inline Optional<ValueType>::Optional(const Optional& other)
	: value_{other.value_}, message_{other.message_}, type_{other.type_} {}

template <class ValueType>
inline Optional<ValueType>::Optional(Optional&& other)
	: value_{std::move(other.value_)}, message_{std::move(other.message_)}, type_{std::move(other.type_)} {}

template <class ValueType>
inline constexpr Optional<ValueType>::operator bool() const { return type_ != Type::Error;}

template <class ValueType>
inline constexpr const ValueType& Optional<ValueType>::value() const & { Q_ASSERT(bool(*this)); return value_; }

template <class ValueType>
inline constexpr ValueType& Optional<ValueType>::value() & { Q_ASSERT(bool(*this)); return value_; }

template <class ValueType>
inline constexpr ValueType&& Optional<ValueType>::value() && { Q_ASSERT(bool(*this)); return std::move(value_); }

template <class ValueType>
inline constexpr bool Optional<ValueType>::hasError() const { return type_ == Type::Error; }
template <class ValueType>
inline QString Optional<ValueType>::error() const { Q_ASSERT(type_ == Type::Error); return message_; }

template <class ValueType>
inline constexpr bool Optional<ValueType>::hasWarning() const { return type_ == Type::Warning; }
template <class ValueType>
inline QString Optional<ValueType>::warning() const { Q_ASSERT(type_ == Type::Warning); return message_; }

} /* namespace InformationScripting */
