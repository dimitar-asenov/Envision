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
};

template<class T>
TypedList<T>::TypedList(::Model::Node* parent, ::Model::Model* model) :
	List(parent, model)
{
}

template<class T>
TypedList<T>::TypedList(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint) :
	List(parent, id, store, partialLoadHint)
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

}

#endif /* TYPEDLISTDEFINITION_H_ */
