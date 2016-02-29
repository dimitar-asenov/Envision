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
template <typename ValueType>
class INFORMATIONSCRIPTING_API Optional
{
		// Since the error constructor takes a QString we disallow usage with QString.
		static_assert(!std::is_same<ValueType, QString>(), "QString is not allowed as paremeter for Optional");
	public:
		Optional(const ValueType& v, const QString& warning = {});
		Optional(ValueType&& v, const QString& warning = {});
		Optional(const QString& errorMessage);
		Optional(QString&& errorMessage);

		Optional(const Optional& other);
		Optional(Optional&& other);

		Optional& operator=(const Optional& other);
		Optional& operator=(Optional&& other);

		explicit operator bool() const;

		const ValueType& value() const &;
		ValueType& value() &;

		ValueType&& value() &&;
		const ValueType&& value() const &&;

		bool hasErrors() const;
		QStringList errors() const;

		bool hasWarnings() const;
		QStringList warnings() const;
		void addWarnings(const QStringList& warnings);
	private:
		enum class Type : int {Value, Warning, Error};
		Q_DECLARE_FLAGS(Types, Type)
		ValueType value_{};
		QStringList warnings_;
		QStringList errors_;
		Types type_{Type::Value};
};

template <typename ValueType>
inline Optional<ValueType>::Optional(const ValueType& v, const QString& warning)
	: value_{v}
{
	if (!warning.isNull())
	{
		warnings_.push_back(warning);
		type_ = Type::Warning;
	}
}

template <typename ValueType>
inline Optional<ValueType>::Optional(ValueType&& v, const QString& warning)
	: value_{std::move(v)}
{
	if (!warning.isNull())
	{
		warnings_.push_back(warning);
		type_ = Type::Warning;
	}
}

template <typename ValueType>
inline Optional<ValueType>::Optional(const QString& errorMessage)
	: type_{Type::Error}
{
	errors_.push_back(errorMessage);
}

template <typename ValueType>
inline Optional<ValueType>::Optional(QString&& errorMessage)
	: type_{Type::Error}
{
	errors_.push_back(std::move(errorMessage));
}

template <typename ValueType>
inline Optional<ValueType>::Optional(const Optional& other)
	: value_{other.value_}, warnings_{other.warnings_}, errors_{other.errors_}, type_{other.type_} {}

template <typename ValueType>
inline Optional<ValueType>::Optional(Optional&& other)
	: value_{std::move(other.value_)}, warnings_{std::move(other.warnings_)},
	  errors_{std::move(other.errors_)}, type_{std::move(other.type_)} {}

template <typename ValueType>
Optional<ValueType>& Optional<ValueType>::operator=(const Optional& other)
{
	value_ = other.value_;
	warnings_ = other.warnings_;
	errors_ = other.errors_;
	type_ = other.type_;
	return *this;
}

template <typename ValueType>
Optional<ValueType>& Optional<ValueType>::operator=(Optional&& other)
{
	value_ = std::move(other.value_);
	warnings_ = std::move(other.warnings_);
	errors_ = std::move(other.errors_);
	type_ = std::move(other.type_);
	return *this;
}

template <typename ValueType>
inline Optional<ValueType>::operator bool() const { return !type_.testFlag(Type::Error);}

template <typename ValueType>
inline const ValueType& Optional<ValueType>::value() const & { Q_ASSERT(bool(*this)); return value_; }

template <typename ValueType>
inline ValueType& Optional<ValueType>::value() & { Q_ASSERT(bool(*this)); return value_; }

template <typename ValueType>
inline ValueType&& Optional<ValueType>::value() && { Q_ASSERT(bool(*this)); return std::move(value_); }

template <typename ValueType>
inline bool Optional<ValueType>::hasErrors() const { return type_.testFlag(Type::Error); }
template <typename ValueType>
inline QStringList Optional<ValueType>::errors() const { Q_ASSERT(hasErrors()); return errors_; }

template <typename ValueType>
inline bool Optional<ValueType>::hasWarnings() const { return type_.testFlag(Type::Warning); }
template <typename ValueType>
inline QStringList Optional<ValueType>::warnings() const { Q_ASSERT(hasWarnings()); return warnings_; }
template <typename ValueType>
inline void Optional<ValueType>::addWarnings(const QStringList& warnings)
{
	if (!warnings.isEmpty())
	{
		type_ |= Type::Warning;
		warnings_ << warnings;
	}
}

}
