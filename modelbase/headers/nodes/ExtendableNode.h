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
#include "commands/ExtendedNodeOptional.h"
#include "Attribute.h"
#include "ModelException.h"

namespace Model {



template<class T>
class MODELBASE_API ExtendableNode: public Node
{
	private:
		QVector<Node*> attributes;
		static QVector<Attribute> attributeDescriptions;

	public:
		ExtendableNode(Node *parent, Model* model) :
			Node(parent, model), attributes(attributeDescriptions.size(), NULL)
		{
			for (int i = 0; i < attributeDescriptions.size(); i++)
				if ( !attributeDescriptions[i].optional() ) attributes[i] = Node::createNewNode(attributeDescriptions[i].type(), this, model);
		}

		ExtendableNode(Node *parent, NodeIdType id, PersistentStore &store, bool) :
			Node(parent, id), attributes(attributeDescriptions.size(), NULL)
		{
			QList<LoadedNode> children = store.loadAllSubNodes(this);

			for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
			{
				int index = attributeDescriptions.indexOf(ln->name);
				if ( index < 0 ) throw ModelException("Node has attribute " + ln->name + " in persistent store, but this attribute is not registered");

				attributes[index] = ln->node;
			}
		}

		virtual ~ExtendableNode()
		{
			for (int i = 0; i < attributes.size(); i++)
				if (attributes[i]) delete attributes[i];
		}

		Node* get(int attributeIndex)
		{
			return attributes[attributeIndex];
		}

		Node* createOptional(int attributeIndex)
		{
			if ( attributeIndex < 0 || attributeIndex >= attributeDescriptions.size() ) throw ModelException("Trying to create an optional attribute with an index out of bounds " + QString::number(attributeIndex));

			if ( attributeDescriptions[attributeIndex].optional() )
			{
				Node* newnode = Node::createNewNode(attributeDescriptions[attributeIndex].type(), this);
				execute(new ExtendedNodeOptional(this, newnode, attributeIndex, &attributes, true));
				return attributes[attributeIndex];
			}
			else
				throw ModelException("Trying to create an optional attribute for an index which is mandatory " + QString::number(attributeIndex));

		}

		void removeOptional(int attributeIndex)
		{
			if ( attributeDescriptions[attributeIndex].optional() )
			{
				execute(new ExtendedNodeOptional(this, attributes[attributeIndex], attributeIndex, &attributes, false));
			}
		}

		void save(PersistentStore &store) const
		{
			for (int i = 0; i < attributes.size(); i++)
				if ( attributes[i] != NULL ) store.saveNode(attributes[i], attributeDescriptions[i].name(), attributeDescriptions[i].partialHint());
		}

		Node* getChild(NodeIdType id) const
		{
			Node* res = NULL;

			for (int i = 0; i < attributes.size(); i++)
				if ( attributes[i]->getId() == id ) res = attributes[i];

			return res;
		}

		Node* getChild(const QString& name) const
		{
			return get(name);
		}

		Node* get(const QString &attributeName) const
		{
			int index = attributeDescriptions.indexOf(attributeName);
			if ( index >= 0 ) return attributes[index];
			return NULL;
		}

		QString getChildReferenceName(const Node* child) const
		{
			int index = attributes.indexOf(const_cast<Node*> (child)); // TODO find a way to do this cleanly
			if ( index >= 0 ) return attributeDescriptions[index].name();
			return NULL;
		}

		bool hasAttribute(const QString& attributeName)
		{
			return attributeDescriptions.contains(attributeName);
		}

		static int registerNewAttribute(const QString &attributeName, const QString &attributeType, bool canBePartiallyLoaded = false, bool isOptional = false)
		{
			if ( attributeDescriptions.contains(attributeName) ) throw ModelException("Trying to register new attribute " + attributeName + " but this name already exists");

			attributeDescriptions.append(Attribute(attributeName, attributeType, isOptional,canBePartiallyLoaded ));

			return attributeDescriptions.size() - 1;
		}
};

template<class T> QVector<Attribute> ExtendableNode<T>::attributeDescriptions;

}

#endif /* EXTENDABLENODE_H_ */
