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
#include "TypedList.h"

namespace Model {

DEFINE_TYPE_ID_DERIVED(TypedList<T>, QString{"TypedListOf"} + T::typeNameStatic(), template<typename T>)

template<typename T>
void TypedList<T>::initType()
{
	Node::registerNodeType(typeNameStatic(),
			[](Node* parent) -> Node* { return TypedList<T>::createDefaultInstance(parent);},
			[](Node *parent, PersistentStore &store, bool loadPartially) -> Node*
						{ return new TypedList<T>{parent, store, loadPartially};});
}

template<typename T>
TypedList<T>* TypedList<T>::createDefaultInstance(Node* parent)
{
	return new TypedList<T>{parent};
}

template<typename T>
TypedList<T>::TypedList(Node* parent) : Super{parent}
{}

template<typename T>
TypedList<T>::TypedList(Node *parent, PersistentStore &store, bool loadPartially) :
Super{parent, store, loadPartially}
{
}

template<typename T>
TypedList<T>* TypedList<T>::clone() const { return new TypedList{*this}; }

template<typename T> T* TypedList<T>::first() const
{
	return List::first<T>();
}
template<typename T> T* TypedList<T>::last() const
{
	return List::last<T>();
}
template<typename T> T* TypedList<T>::at(int i) const
{
	return List::at<T>(i);
}

template<typename T> bool TypedList<T>::replaceChild(Node* child, Node* replacement)
{
	if (!dynamic_cast<T*>(replacement)) return false;
	else return List::replaceChild(child, replacement);
}

template<typename T> Node* TypedList<T>::createDefaultElement()
{
	return T::createDefaultInstance(nullptr);
}

template<typename T> int TypedList<T>::lowerTypeBoundForElements() const { return T::typeIdStatic();}

}
