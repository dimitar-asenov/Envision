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

#include "core_api.h"

namespace Core {

class CORE_API AdapterManager {
	public:
		using AdapterCreationFunction = void* (*)(void* adaptee);

		template <typename Adapter, typename Adaptee>
		static void registerAdapter( Adapter* (*creationFunction)(Adaptee* adaptee) );

		template <typename Adapter>
		static void registerDefaultAdapter( Adapter* (*creationFunction)(typename Adapter::BaseAdapteeType* adaptee) );

		template <typename Adapter, typename Adaptee>
		static Adapter* adapt(Adaptee* object);

		/**
		 * \brief Creates an instance of \a Adapter using it's Adapter::Adapter(Adaptee* a) constructor.
		 *
		 * This is a convenience method for creating adapter functions.
		 */
		template <typename AdapterBase, typename Adapter, typename Adaptee>
		static AdapterBase* createFrom(Adaptee* a);

		template <typename AdapterBase, typename Adapter, typename Adaptee>
		static void registerAdapterViaConstructor( );

	private:
		using TypeIdType = std::size_t;
		using AdapterKey =  QPair<TypeIdType, TypeIdType>;

		template <typename type>
		static TypeIdType typeId();

		template <typename type>
		static TypeIdType typeId( type* object);

		static QHash<AdapterKey, AdapterCreationFunction>& adapters();
		static QHash<TypeIdType, AdapterCreationFunction>& defaultAdapters();
};

template <typename type> AdapterManager::TypeIdType AdapterManager::typeId()
{
	return typeid(type).hash_code();
}

template <typename type> AdapterManager::TypeIdType AdapterManager::typeId( type* object)
{
	if (object)	return typeid(*object).hash_code();
	else return typeId<type>();
}

template <typename Adapter, typename Adaptee>
void AdapterManager::registerAdapter( Adapter* (*creationFunction)(Adaptee* adaptee) )
{
	adapters().insert(qMakePair(typeId<Adapter>(), typeId<Adaptee>()),
			reinterpret_cast<AdapterCreationFunction>(creationFunction));
}

template <typename Adapter>
void AdapterManager::registerDefaultAdapter( Adapter* (*creationFunction)(typename Adapter::BaseAdapteeType* adaptee) )
{
	defaultAdapters().insert( typeId<Adapter>(), reinterpret_cast<AdapterCreationFunction>(creationFunction));
}

template <typename Adapter, typename Adaptee> Adapter* AdapterManager::adapt(Adaptee* object)
{
	// Try to find a specific adapter
	auto specific = adapters().find( qMakePair(typeId<Adapter>(), typeId<Adaptee>(object)) );
	if (specific != adapters().end())
		return reinterpret_cast<Adapter*> ( (*specific)( reinterpret_cast<void*>(object) ) );

	// Try to find a default adapter
	auto defaultAdp = defaultAdapters().find( typeId<Adapter>() );
	if (defaultAdp != defaultAdapters().end())
	{
		// Force check to make sure we are adapting the right object type
		typename Adapter::BaseAdapteeType* obj = object;

		return reinterpret_cast<Adapter*> ( (*defaultAdp)( reinterpret_cast<void*>(obj) ) );
	}

	return nullptr;
}

template <typename AdapterBase, typename Adapter, typename Adaptee >
AdapterBase* AdapterManager::createFrom(Adaptee* a)
{
	return new Adapter{a};
}

template <typename AdapterBase, typename Adapter, typename Adaptee>
void AdapterManager::registerAdapterViaConstructor( )
{
	registerAdapter<AdapterBase, Adaptee>( createFrom<AdapterBase, Adapter, Adaptee> );
}

}
