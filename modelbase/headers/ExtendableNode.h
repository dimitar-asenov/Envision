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

namespace Model {

template<class T>
class MODELBASE_API ExtendableNode: public Model::Node
{
	private:
		QVector<Node*> attributes;
		static QVector<QString> attributeNames;
		static QVector<QString> attributeTypes;

	protected:
		Node* get(int attributeIndex);

	public:
		ExtendableNode(Node *parent) :
			Node(parent), attributes(attributeNames.size(), NULL)
		{
			for (int i = 0; i < attributeNames; i++)
				attributes[i] = Node::createNewNode(attributeTypes[i], this);
		}

		ExtendableNode(Node *parent, IdType id, PersistentStore &store) :
			Node(parent, id), attributes(attributeNames.size(), NULL)
		{
			while ( store.hasMoreSubNodes() )
			{
				LoadedNode ln = getNextSubNode();

				int index = attributeNames.indexOf(ln.name);
				if (index < 0) continue; //TODO throw an exception

				attributes[index] = ln.node;
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
				store.saveSubNode(attributes[i], attributeNames[i]);
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

		static int registerNewAttribute(const QString &attributeName, const QString &attributeType)
		{
			if ( attributeNames.contains(attributeName) ) return attributeNames.indexOf(attributeName);
			//TODO include proper exception handling

			attributeNames.append(attributeName);
			attributeTypes.append(attributeType);

			return attributeNames.size() - 1;
		}
};

template<class T> QVector<QString> ExtendableNode<T>::attributeNames;
template<class T> QVector<QString> ExtendableNode<T>::attributeTypes;

}

#endif /* EXTENDABLENODE_H_ */
