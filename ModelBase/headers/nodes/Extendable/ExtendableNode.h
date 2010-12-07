/***********************************************************************************************************************
 * ExtendableNode.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDABLENODE_H_
#define EXTENDABLENODE_H_

#include "AttributeChain.h"
#include "../Node.h"
#include "../../ModelException.h"
#include "../../commands/ExtendedNodeOptional.h"

#include <QtCore/QString>
#include <QtCore/QVector>

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

		template<class T>
		T* createOptional(const ExtendableIndex &attributeIndex, const QString& type = QString())
		{
			if ( !attributeIndex.isValid() ) throw ModelException("Trying to create an optional attribute with an invalid Index");

			if ( meta.getAttribute(attributeIndex).optional() )
			{
				QString creationType = meta.getAttribute(attributeIndex).type();
				if ( !type.isEmpty() ) creationType = type;

				Node* nodeGeneric = Node::createNewNode(creationType, this);
				T* nodeSpecific = dynamic_cast<T*> (nodeGeneric);

				if ( nodeSpecific == NULL )
				{
					if (nodeGeneric) delete nodeGeneric;
					throw ModelException("Could not create optional node with the type " + creationType
							+ ". This type is not compatible with the expected node type of this attribute.");
				}

				execute(new ExtendedNodeOptional(this, nodeSpecific, attributeIndex, &subnodes, true));

				return nodeSpecific;
			}
			else
				throw ModelException("Trying to create a non-optional attribute");
		}

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
		static ExtendableIndex registerNewAttribute(const QString &attributeName, const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent)
		{
			if ( getMetaData<T> ().hasAttribute(attributeName) ) throw ModelException("Trying to register new attribute " + attributeName + " but this name already exists");

			getMetaData<T> ().append(Attribute(attributeName, attributeType, isOptional, canBePartiallyLoaded, isPersistent));

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
