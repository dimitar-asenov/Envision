/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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
#include "TypedList.h"

namespace Model {

DEFINE_TYPE_ID_DERIVED(TypedList<T>, TypedList<T>::elementInitializationRegistry,
		QString("TypedListOf") + T::typeNameStatic(), template<class T>)

template<class T>
Core::InitializationRegistry& TypedList<T>::elementInitializationRegistry()
{
	return T::initializationRegistry();
}

template<class T>
void TypedList<T>::initType()
{
	TypedList<T>::typeIdVariable() = Node::registerNodeType(typeNameStatic(),
			[](Node* parent) -> Node* { return TypedList<T>::createDefaultInstance(parent);},
			[](Node *parent, PersistentStore &store, bool partialLoadHint) -> Node*
						{ return new TypedList<T>(parent, store, partialLoadHint);});
}

template<class T>
TypedList<T>* TypedList<T>::createDefaultInstance(Node* parent)
{
	return new TypedList<T>(parent);
}

template<class T>
TypedList<T>::TypedList(::Model::Node* parent) : Super(parent)
{}

template<class T>
TypedList<T>::TypedList(::Model::Node *parent, ::Model::PersistentStore &store, bool partialLoadHint) :
Super(parent, store, partialLoadHint)
{
}

template<class T> T* TypedList<T>::first()
{
	return List::first<T>();
}
template<class T> T* TypedList<T>::last()
{
	return List::last<T>();
}
template<class T> T* TypedList<T>::at(int i)
{
	return List::at<T>(i);
}

template<class T> bool TypedList<T>::replaceChild(Node* child, Node* replacement)
{
	if (!dynamic_cast<T*>(replacement)) return false;
	else return List::replaceChild(child, replacement);
}

template<class T> Node* TypedList<T>::createDefaultElement()
{
	return T::createDefaultInstance(nullptr);
}

template<class T> int TypedList<T>::lowerTypeBoundForElements() const { return T::typeIdStatic();}

}
