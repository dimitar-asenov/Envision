/***********************************************************************************************************************
 * List.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LIST_H_
#define LIST_H_

#include "Node.h"
#include "../Model.h"
#include "../commands/ListInsert.h"
#include "../ModelException.h"

#include <QtCore/QVector>

namespace Model {

class Text;

class List: public Node
{
	private:
		QVector<Node*> nodes;
		Text* referenceName;

		void loadSubNodes(QList<LoadedNode>& nodeList);

		static int typeId;

	public:
		List(Node *parent, Model* model);
		List(Node *parent, NodeIdType id, PersistentStore &store, bool partialHint);
		virtual ~List();

		void save(PersistentStore &store) const;
		void loadFully(PersistentStore &store);

		QString getTypeName() const;
		int getTypeId() const;
		static int getTypeIdStatic();

		Node* getChild(NodeIdType id);
		Node* getChild(const QString& name);

		QString getReferenceName() const;
		QString getChildReferenceName(const Node* child) const;

		void setReferenceName(const QString &name);

		int size();
		template <class T> T* first();
		template <class T> T* last();
		template <class T> T* at(int i);

		template <class T> T* append();
		template <class T> T* prepend();
		template <class T> T* insert(int position);

		void remove(int index);
		void remove(Node* instance);

		static void registerNodeType();
};

template <class T> T* List::first()
{
	if (!fullyLoaded) loadFully(* (getModel()->getLastUsedStore()));

	if ( nodes.isEmpty() ) throw ModelException("Trying to access the first element of an empty list.");
	return static_cast<T*> (nodes.first());
}

template <class T> T* List::last()
{
	if (!fullyLoaded) loadFully(* (getModel()->getLastUsedStore()));

	if ( nodes.isEmpty() ) throw ModelException("Trying to access the last element of an empty list.");
	return static_cast<T*> (nodes.last());
}

template <class T> T* List::at(int i)
{
	if (!fullyLoaded) loadFully(* (getModel()->getLastUsedStore()));

	return static_cast<T*> (nodes[i]);
}

template <class T>
T* List::append()
{
	if (!fullyLoaded) loadFully(* (getModel()->getLastUsedStore()));

	return insert<T>(nodes.size());
}

template <class T>
T* List::prepend()
{
	if (!fullyLoaded) loadFully(* (getModel()->getLastUsedStore()));

	return insert<T>(0);
}

template <class T>
T* List::insert(int position)
{
	if (!fullyLoaded) loadFully(* (getModel()->getLastUsedStore()));

	T* newNode = new T(this, NULL);
	if (! Node::isTypeRegistered(newNode->getTypeName())) throw ModelException("Trying to create a list entry of an unregistered type.");

	execute(new ListInsert(this, nodes, newNode, position));
	return newNode;
}

}

#endif /* LIST_H_ */
