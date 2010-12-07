/***********************************************************************************************************************
 * List.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LIST_H_
#define LIST_H_

#include "Node.h"
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

		Node* getChild(NodeIdType id) const;
		Node* getChild(const QString& name) const;

		QString getReferenceName() const;
		QString getChildReferenceName(const Node* child) const;

		void setReferenceName(const QString &name);

		int size() const;
		Node* first();
		const Node* first() const;

		Node* last();
		const Node* last() const;

		Node* operator[](int i);
		const Node* operator[](int i) const;

		Node* append(const QString& type);
		Node* prepend(const QString& type);
		Node* insert(const QString& type, int position);

		void remove(int index);
		void remove(Node* instance);

		static void registerNodeType();
};

}

#endif /* LIST_H_ */
