/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#pragma once

#include "../oomodel_api.h"
#include "../typesystem/TypeSystem.h"

namespace OOModel {

class OOMODEL_API Type {
	public:

		enum Qualifier {
			CONST = 0x1,
			VOLATILE = 0x2
		};
		using Qualifiers = QFlags<Qualifier>;

		Type(bool isValueType);
		Type(const Type& other);
		virtual ~Type();

		/**
		 * \brief Returns true if this is an error type.
		 *
		 * The default implementation returns false.
		 */
		virtual bool isError() const;

		/**
		 * \brief Returns true if the this type belongs to an expression representing a value.
		 *
		 * Expressions can represent values e.g. '5', 'a+b' or types e.g. 'int[]'. If this type object represents a value
		 * expression this method returns true, otherwise it returns false.
		 */
		bool isValueType() const;
		void setValueType(bool isValueType);

		virtual bool equals(const Type* other) const = 0;
		virtual Type* clone() const = 0;

		Qualifiers qualifiers() const;
		void setQualifiers(Qualifiers q, bool enable = true);

		TypeSystem::TypeRelations relationTo(const Type* other) const;

	private:
		bool isValueType_;
		Qualifiers qualifiers_{};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Type::Qualifiers)

inline bool Type::isValueType() const { return isValueType_; }
inline void Type::setValueType(bool isValueType) { isValueType_ = isValueType; }

inline Type::Qualifiers Type::qualifiers() const { return qualifiers_;}
inline void Type::setQualifiers(Qualifiers q, bool enable) {if (enable) qualifiers_ |= q; else qualifiers_ &= (~q);}


}
