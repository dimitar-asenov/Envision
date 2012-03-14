/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * TypedListDefinition.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TYPEDLISTDEFINITION_H_
#define TYPEDLISTDEFINITION_H_

#ifndef TYPEDLIST_H_
#define TYPEDLIST_H_
#endif /* TYPEDLIST_H_ */

#include "List.h"

namespace Model {

template <class T>
class Q_DECL_EXPORT TypedList: public List
{
	NODE_DECLARE_STANDARD_METHODS(TypedList)

	public:
		T* first();
		T* last();
		T* at(int i);

		virtual bool replaceChild(Node* child, Node* replacement, bool releaseOldChild = true);
};

template<class T>
TypedList<T>::TypedList(::Model::Node* parent) :
	List(parent)
{
}

template<class T>
TypedList<T>::TypedList(::Model::Node *parent, ::Model::PersistentStore &store, bool partialLoadHint) :
	List(parent, store, partialLoadHint)
{
}

template<class T> int TypedList<T>::typeId_ = -1; /* This must be set to the result of Node::registerNodeType */

template<class T>
const QString& TypedList<T>::typeName() const
{
	return typeNameStatic();
}

template<class T>
int TypedList<T>::typeId() const
{
	return typeId_;
}

template<class T>
int TypedList<T>::typeIdStatic()
{
	return typeId_;
}

template<class T>
const QString& TypedList<T>::typeNameStatic()
{
	static QString typeName_(QString("TypedListOf") + T::typeNameStatic());
	return typeName_;
}

template<class T>
void TypedList<T>::registerNodeType()
{
	typeId_ = Node::registerNodeType(typeNameStatic(), ::Model::createNewNode<TypedList<T> >, ::Model::createNodeFromPersistence<TypedList<T> >);
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

template<class T> bool TypedList<T>::replaceChild(Node* child, Node* replacement, bool releaseOldChild)
{
	if (!dynamic_cast<T*>(replacement)) return false;
	else return List::replaceChild(child, replacement, releaseOldChild);
}

}

#endif /* TYPEDLISTDEFINITION_H_ */
