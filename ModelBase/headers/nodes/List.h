/***********************************************************************************************************************
 * List.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LIST_H_
#define LIST_H_

#include "Node.h"
#include "nodeMacros.h"
#include "../Model.h"
#include "../commands/ListInsert.h"
#include "../ModelException.h"
#include "../persistence/ClipboardStore.h"

#include <QtCore/QVector>

namespace Model {

class Text;

class List: public Node
{
	NODE_DECLARE_STANDARD_METHODS(List)

	private:
		QVector<Node*> nodes;
		Text* referenceName_;

		void loadSubNodes(QList<LoadedNode>& nodeList);

	public:
		virtual ~List();

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
		virtual void loadFully(PersistentStore &store);

		virtual Node* child(NodeIdType id);
		virtual Node* child(const QString& name);

		virtual QString referenceName() const;
		virtual QString childReferenceName(const Node* child) const;

		void setReferenceName(const QString &name);

		int size();
		template <class T> T* first();
		template <class T> T* last();
		template <class T> T* at(int i);

		int indexOf(const Node* item) const;

		template <class T> T* append();
		template <class T> T* prepend();
		template <class T> T* insert(int position);
		void paste(ClipboardStore& clipboard, int position);

		void remove(int index);
		void remove(Node* instance);
		void clear();
};

template <class T> T* List::first()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	if ( nodes.isEmpty() ) throw ModelException("Trying to access the first element of an empty list.");
	return static_cast<T*> (nodes.first());
}

template <class T> T* List::last()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	if ( nodes.isEmpty() ) throw ModelException("Trying to access the last element of an empty list.");
	return static_cast<T*> (nodes.last());
}

template <class T> T* List::at(int i)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	return static_cast<T*> (nodes[i]);
}

template <class T>
T* List::append()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	return insert<T>(nodes.size());
}

template <class T>
T* List::prepend()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	return insert<T>(0);
}

template <class T>
T* List::insert(int position)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	T* newNode = new T(this, NULL);
	if (! Node::isTypeRegistered(newNode->typeName())) throw ModelException("Trying to create a list entry of an unregistered type.");

	execute(new ListInsert(this, nodes, newNode, position));
	return newNode;
}

}

#endif /* LIST_H_ */
