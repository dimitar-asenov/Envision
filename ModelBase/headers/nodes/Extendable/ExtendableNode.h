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

		static int typeId_;
		static int nextExtensionId_;

	protected:
		static ExtendableIndex registerNewAttribute(AttributeChain& metaData, const QString &attributeName,
				const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent);

		static ExtendableIndex registerNewAttribute(AttributeChain& metaData, const Attribute& attribute);

		virtual AttributeChain& topLevelMeta();
	public:

		ExtendableNode(Node *parent, Model* model, AttributeChain& metaData);
		ExtendableNode(Node *parent, NodeIdType id, PersistentStore &store, bool partialHint, AttributeChain& metaData);

		ExtendableNode(Node *parent, Model* model);
		ExtendableNode(Node *parent, NodeIdType id, PersistentStore &store, bool partialHint);

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

		virtual const QString& typeName() const;
		virtual int typeId() const;
		static const QString& typeNameStatic();
		static int typeIdStatic();
		static void registerNodeType();
		static int registerExtensionId();

		template <class T> T* extension();
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

	execute(new ExtendedNodeChild(this, nodeSpecific, attributeIndex, &subnodes));

	return nodeSpecific;
}

inline int ExtendableNode::typeIdStatic() { return typeId_;}
inline int ExtendableNode::registerExtensionId() { return nextExtensionId_++; }

template <class T> T* ExtendableNode::extension()
{
	AttributeChain& topMeta = topLevelMeta();
	if (topMeta.hasExtensionInHierarchy(T::extensionId())) return new T(this, topMeta.extension(T::extensionId()));
	else return NULL;
}

}

#endif /* EXTENDABLENODE_H_ */
