/***********************************************************************************************************************
 * ExtendableNode.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/ExtendableNode.h"
#include "commands/ExtendedNodeOptional.h"

namespace Model {

ExtendableNode::ExtendableNode(Node *parent, Model* model, AttributeChain& metaData) :
	Node(parent, model), meta(metaData), subnodes(meta.getNumLevels())
{
	for (int level = 0; level < meta.getNumLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.getLevel(level);
		subnodes[level] = QVector<Node*> (currentLevel->size(), NULL);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( !(*currentLevel)[i].optional() ) subnodes[level][i] = Node::createNewNode((*currentLevel)[i].type(), this, model);
	}
}

ExtendableNode::ExtendableNode(Node *parent, NodeIdType id, PersistentStore &store, bool, AttributeChain& metaData) :
	Node(parent, id), meta(metaData), subnodes(meta.getNumLevels())
{
	QList<LoadedNode> children = store.loadAllSubNodes(this);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		ExtendableIndex index = meta.getIndexForAttribute(ln->name);
		if ( !index.isValid() ) throw ModelException("Node has attribute " + ln->name + " in persistent store, but this attribute is not registered");

		subnodes[index.level()][index.index()] = ln->node;
	}
}

ExtendableNode::~ExtendableNode()
{
	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i] ) delete subnodes[level][i];
}

Node* ExtendableNode::get(const ExtendableIndex &attributeIndex)
{
	return subnodes[attributeIndex.level()][attributeIndex.index()];
}

Node* ExtendableNode::get(const QString &attributeName) const
{
	ExtendableIndex index = meta.getIndexForAttribute(attributeName);
	if ( index.isValid() ) return subnodes[index.level()][index.index()];
	return NULL;
}

Node* ExtendableNode::getChild(NodeIdType id) const
{
	Node* res = NULL;

	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i]->getId() == id ) res = subnodes[level][i];

	return res;
}

Node* ExtendableNode::getChild(const QString& name) const
{
	return get(name);
}

QString ExtendableNode::getChildReferenceName(const Node* child) const
{
	for (int level = 0; level < subnodes.size(); ++level)
	{
		int index = subnodes[level].indexOf(const_cast<Node*> (child)); // TODO find a way to do this cleanly
		if ( index >= 0 ) return meta.getAttribute(ExtendableIndex(level, index)).name();
	}

	return QString();
}

bool ExtendableNode::hasAttribute(const QString& attributeName)
{
	return meta.hasAttribute(attributeName);
}

Node* ExtendableNode::createOptional(const ExtendableIndex &attributeIndex, const QString& type)
{
	if ( !attributeIndex.isValid() ) throw ModelException("Trying to create an optional attribute with an invalid Index");

	if ( meta.getAttribute(attributeIndex).optional() )
	{
		QString creationType = meta.getAttribute(attributeIndex).type();
		if ( !type.isEmpty()) creationType = type;

		Node* newnode = Node::createNewNode(creationType, this);
		execute(new ExtendedNodeOptional(this, newnode, attributeIndex, &subnodes, true));

		return subnodes[attributeIndex.level()][attributeIndex.index()];
	}
	else
		throw ModelException("Trying to create a non-optional attribute");


}

void ExtendableNode::removeOptional(const ExtendableIndex &attributeIndex)
{
	if ( meta.getAttribute(attributeIndex).optional() )
	{
		execute(new ExtendedNodeOptional(this, subnodes[attributeIndex.level()][attributeIndex.index()], attributeIndex, &subnodes, false));
	}
	else
			throw ModelException("Trying to remove a non-optional attribute");
}

void ExtendableNode::save(PersistentStore &store) const
{
	for (int level = 0; level < meta.getNumLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.getLevel(level);
		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes[level][i] != NULL ) store.saveNode(subnodes[level][i], currentLevel->at(i).name(), currentLevel->at(i).partialHint());
	}

}

}

