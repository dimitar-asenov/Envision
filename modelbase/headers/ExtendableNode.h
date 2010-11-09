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

namespace Model {

template<class T>
class MODELBASE_API ExtendableNode: public Model::Node
{
	private:
		QVector<Node*> attributes;
		static QVector<QString> attributeNames;
		static QVector<QString> attributeTypes;
		static QVector<bool> attributeOptional;

	protected:
		Node* get(int attributeIndex)
		{
			return attributes[attributeIndex];
		}

		Node* createOptional(int attributeIndex)
		{
			if ( attributeOptional(attributeIndex) )
			{
				Node* newnode = Node::createNewNode(attributeTypes[attributeIndex], this);
				execute(new ExtendedNodeOptional(this, newnode, attributeIndex, &attributes, true));
				return attributes[attributeIndex];
			}

			// TODO throw exception instead of returning null
			return NULL;
		}

		void removeOptional(int attributeIndex)
		{
			if ( attributeOptional(attributeIndex) )
			{
				execute(new ExtendedNodeOptional(this, attributes[attributeIndex], attributeIndex, &attributes, false));
			}
		}

	public:
		ExtendableNode(Node *parent) :
			Node(parent), attributes(attributeNames.size(), NULL)
		{
			for (int i = 0; i < attributeNames; i++)
				if ( !attributeOptional[i] ) attributes[i] = Node::createNewNode(attributeTypes[i], this);
		}

		ExtendableNode(Node *parent, IdType id, PersistentStore &store, bool) :
			Node(parent, id), attributes(attributeNames.size(), NULL)
		{
			QList<LoadedNode> children = p.loadAllSubNodes(this);

			for (QList<LoadedNode>::iterator ln = children.begin(); attr != children.end(); attr++)
			{
				int index = attributeNames.indexOf(ln->name);
				if ( index < 0 ) continue; //TODO throw an exception

				attributes[index] = ln->node;
			}
		}

		virtual ~ExtendableNode()
		{
			for (int i = 0; i < attributes.size(); i++)
				delete attributes[i];
		}

		void save(PersistentStore &store, QString &name)
		{
			for (int i = 0; i < attributes.size(); i++)
				store.saveNode(attributes[i], attributeNames[i]);
		}

		Node* getChild(IdType id)
		{
			Node* res = NULL;

			for (int i = 0; i < attributes.size(); i++)
				if ( attributes[i]->getId() == id ) res = attributes[i];

			return res;
		}

		Node* get(const QString &attributeName)
		{
			int index = attributeNames.indexOf(attributeName);
			if ( index >= 0 ) return attributes[index];
			return NULL;
		}

		bool hasAttribute(const QString& attributeName)
		{
			return attributeNames.contains(attributeName);
		}

		const QString& getTypeName()
		{
			return "ExtendableNode";
		}

		static int registerNewAttribute(const QString &attributeName, const QString &attributeType, bool isOptional = false)
		{
			if ( attributeNames.contains(attributeName) ) return attributeNames.indexOf(attributeName);
			//TODO include proper exception handling

			attributeNames.append(attributeName);
			attributeTypes.append(attributeType);
			attributeOptional.append(isOptional);

			return attributeNames.size() - 1;
		}
};

template<class T> QVector<QString> ExtendableNode<T>::attributeNames;
template<class T> QVector<QString> ExtendableNode<T>::attributeTypes;
template<class T> QVector<bool> ExtendableNode<T>::attributeOptional;

}

#endif /* EXTENDABLENODE_H_ */
