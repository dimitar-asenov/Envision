/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../modelbase_api.h"

namespace Model {

class MODELBASE_API AdapterManager {
	public:
		typedef void* (*AdapterCreationFunction)(void* adaptee);

		template <class Adapter, class Adaptee>
		static void registerAdapter( Adapter* (*creationFunction)(Adaptee* adaptee) );

		template <class Adapter, class Adaptee>
		static Adapter* adapt(Adaptee* object);

		/**
		 * \brief Creates an instance of \a Adapter using it's Adapter::Adapter(Adaptee* a) constructor.
		 *
		 * This is a convenience method for creating adapter functions.
		 */
		template <class AdapterBase, class Adapter, class Adaptee>
		static AdapterBase* createFrom(Adaptee* a);

		template <class AdapterBase, class Adapter, class Adaptee>
		static void registerAdapterViaConstructor( );

	private:
		typedef std::size_t TypeIdType;
		typedef QPair<TypeIdType, TypeIdType> AdapterKey;

		template <class type>
		static TypeIdType typeId();

		template <class type>
		static TypeIdType typeId( type* object);

		static QHash<AdapterKey, AdapterCreationFunction>& adapters();
};

template <class type> AdapterManager::TypeIdType AdapterManager::typeId()
{
	return typeid(type).hash_code();
}

template <class type> AdapterManager::TypeIdType AdapterManager::typeId( type* object)
{
	if (object)	return typeid(*object).hash_code();
	else return typeId<type>();
}

template <class Adapter, class Adaptee>
void AdapterManager::registerAdapter( Adapter* (*creationFunction)(Adaptee* adaptee) )
{
	adapters().insert(qMakePair(typeId<Adapter>(), typeId<Adaptee>()),
			reinterpret_cast<AdapterCreationFunction>(creationFunction));
}

template <class Adapter, class Adaptee> Adapter* AdapterManager::adapt(Adaptee* object)
{
	auto cf = adapters().find( qMakePair(typeId<Adapter>(), typeId<Adaptee>(object)) );
	if (cf == adapters().end()) return nullptr;
	return reinterpret_cast<Adapter*> ( (*cf)( reinterpret_cast<void*>(object) ) );
}

template <class AdapterBase, class Adapter, class Adaptee > AdapterBase* AdapterManager::createFrom(Adaptee* a)
{
	return new Adapter(a);
}

template <class AdapterBase, class Adapter, class Adaptee> void AdapterManager::registerAdapterViaConstructor( )
{
	registerAdapter<AdapterBase, Adaptee>( createFrom<AdapterBase, Adapter, Adaptee> );
}

} /* namespace Model */