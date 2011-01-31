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
#include "../../commands/ExtendedNodeChild.h"

#include "Core/headers/global.h"
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QPair>
#include <QtCore/QList>

namespace Model {

class MODELBASE_API ExtendableNode: public Node
{
	private:
		AttributeChain& meta;
		QVector<QVector<Node*> > subnodes;

		void removeAllNodes();
		void verifyHasAllMandatoryAttributes();

	protected:
		static ExtendableIndex registerNewAttribute(AttributeChain& metaData, const QString &attributeName,
				const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent);

		static ExtendableIndex registerNewAttribute(AttributeChain& metaData, const Attribute& attribute);
	public:

		ExtendableNode(Node *parent, Model* model, AttributeChain& metaData);
		ExtendableNode(Node *parent, NodeIdType id, PersistentStore &store, bool partialHint, AttributeChain& metaData);
		virtual ~ExtendableNode();

		Node* get(const ExtendableIndex &attributeIndex) const;
		Node* get(const QString &attributeName) const;

		virtual Node* child(NodeIdType id);
		virtual Node* child(const QString& name);

		virtual QString childReferenceName(const Node* child) const;

		template<class T>
		T* set(const ExtendableIndex &attributeIndex, const QString& type = QString());

		void removeOptional(const ExtendableIndex &attributeIndex);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);

		bool hasAttribute(const QString& attributeName);

		QList< QPair<QString, Node*> > getAllAttributes(bool includeNullValues = false);

		static AttributeChain& getMetaData();
};

template<class T>
T* ExtendableNode::set(const ExtendableIndex &attributeIndex, const QString& type)
{
	if ( !attributeIndex.isValid() ) throw ModelException("Trying to set an attribute with an invalid Index");

	QString creationType = meta.attribute(attributeIndex).type();
	if ( !type.isEmpty() ) creationType = type;

	Node* nodeGeneric = Node::createNewNode(creationType, this);
	T* nodeSpecific = dynamic_cast<T*> (nodeGeneric);

	if ( nodeSpecific == NULL )
	{
		if (nodeGeneric) SAFE_DELETE(nodeGeneric);
		throw ModelException("Could not create a node with the type " + creationType
				+ ". This type is not compatible with the expected node type of this attribute.");
	}

	if (subnodes[attributeIndex.level()][attributeIndex.index()]) removeOptional(attributeIndex);

	execute(new ExtendedNodeChild(this, nodeSpecific, attributeIndex, &subnodes, true));

	return nodeSpecific;
}

}

#endif /* EXTENDABLENODE_H_ */
