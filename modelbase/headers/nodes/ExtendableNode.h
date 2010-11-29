/***********************************************************************************************************************
 * ExtendableNode.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDABLENODE_H_
#define EXTENDABLENODE_H_

#include "Node.h"
#include <QVector>
#include <QString>
#include "AttributeChain.h"
#include "ModelException.h"

namespace Model {

class MODELBASE_API ExtendableNode: public Node
{
	private:
		AttributeChain& meta;
		QVector<QVector<Node*> > subnodes;

	protected:

		template<class Self, class Parent>
		static void setParentMeta()
		{
			getMetaData<Self> ().setParent(&getMetaData<Parent> ());
		}

	public:

		ExtendableNode(Node *parent, Model* model, AttributeChain& metaData);
		ExtendableNode(Node *parent, NodeIdType id, PersistentStore &store, bool, AttributeChain& metaData);
		virtual ~ExtendableNode();

		Node* get(const ExtendableIndex &attributeIndex);
		Node* get(const QString &attributeName) const;

		Node* getChild(NodeIdType id) const;
		Node* getChild(const QString& name) const;

		QString getChildReferenceName(const Node* child) const;

		Node* createOptional(const ExtendableIndex &attributeIndex, const QString& type = QString());
		void removeOptional(const ExtendableIndex &attributeIndex);

		void save(PersistentStore &store) const;

		bool hasAttribute(const QString& attributeName);

		template<class T>
		static AttributeChain& getMetaData()
		{
			static AttributeChain descriptions;
			return descriptions;
		}

		template<class T>
		static ExtendableIndex registerNewAttribute(const QString &attributeName, const QString &attributeType, bool canBePartiallyLoaded = false, bool isOptional = false)
		{
			if ( getMetaData<T> ().hasAttribute(attributeName) ) throw ModelException("Trying to register new attribute " + attributeName + " but this name already exists");

			getMetaData<T> ().append(Attribute(attributeName, attributeType, isOptional, canBePartiallyLoaded));

			return ExtendableIndex(getMetaData<T> ().getNumLevels() - 1, getMetaData<T> ().size() - 1);
		}
};

template<class T> Node* createNewExtendableNode(Node* parent, Model* model)
{
	return new T(parent, model, ExtendableNode::getMetaData<T>());
}

template<class T> Node* createExtendableNodeFromPersistence(Node *parent, NodeIdType id, PersistentStore &store, bool partialLoadHint)
{
	return new T(parent, id, store, partialLoadHint, ExtendableNode::getMetaData<T>());
}

}

#endif /* EXTENDABLENODE_H_ */
