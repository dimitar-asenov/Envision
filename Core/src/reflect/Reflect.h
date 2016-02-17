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

#include "../core_api.h"

namespace Core {

template <typename Base>
class Reflect : public Base
{
	protected:
		using Super = Reflect<Base>;
	public:
		using Base::Base;
};

QString CORE_API readNamePart(const char* part, const char*& nextPart);
}

/**
 * This alias is deliberitely outside of the namespace to make using this template less verbose
 */
template <typename Base> using Super = Core::Reflect<Base>;

template <typename Derived, typename Base>
inline Derived* DCast(Base* b)
{
	if (b && b->isSubtypeOf(Derived::typeIdStatic()))
		return static_cast<Derived*>(b);
	else
		return nullptr;
}

class CORE_API TypeName {
	public:
		QString className_;
		QString namespace_;
};

template <typename T>
TypeName typeName()
{
	TypeName name;

	auto nextPart = typeid(T).name();

	// For now we only support namespaces and plain classes
	Q_ASSERT(nextPart[0] == 'N' || (nextPart[0] >='0' && nextPart[0] <='9'));

	if (*nextPart == 'N')
		name.namespace_ = Core::readNamePart(nextPart+1, nextPart);

	//Only one namespace is supported for now
	name.className_ = Core::readNamePart(nextPart, nextPart);

	if (!name.namespace_.isEmpty()) Q_ASSERT(*nextPart == 'E');

	return name;
}
