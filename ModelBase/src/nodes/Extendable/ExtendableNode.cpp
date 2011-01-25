/***********************************************************************************************************************
 * ExtendableNode.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Extendable/ExtendableNode.h"

#include "Core/headers/global.h"

namespace Model {

ExtendableNode::ExtendableNode(Node *parent, Model* model, AttributeChain& metaData) :
	Node(parent, model), meta(metaData), subnodes(meta.numLevels())
{
	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);
		subnodes[level] = QVector<Node*> (currentLevel->size(), NULL);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( !(*currentLevel)[i].optional() ) subnodes[level][i] = Node::createNewNode((*currentLevel)[i].type(), this, model);
	}
}

ExtendableNode::ExtendableNode(Node *parent, NodeIdType id, PersistentStore &store, bool, AttributeChain& metaData) :
	Node(parent, id), meta(metaData), subnodes(meta.numLevels())
{
	for (int level = 0; level < meta.numLevels(); ++level)
		subnodes[level] = QVector<Node*> (meta.level(level)->size(), NULL);

	QList<LoadedNode> children = store.loadAllSubNodes(this);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		ExtendableIndex index = meta.indexForAttribute(ln->name);
		if ( !index.isValid() ) throw ModelException("Node has attribute " + ln->name + " in persistent store, but this attribute is not registered");

		subnodes[index.level()][index.index()] = ln->node;
	}

	verifyHasAllMandatoryAttributes();
}

ExtendableNode::~ExtendableNode()
{
	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i] ) SAFE_DELETE( subnodes[level][i] );
}

Node* ExtendableNode::get(const ExtendableIndex &attributeIndex)
{
	return subnodes[attributeIndex.level()][attributeIndex.index()];
}

Node* ExtendableNode::get(const QString &attributeName) const
{
	ExtendableIndex index = meta.indexForAttribute(attributeName);
	if ( index.isValid() ) return subnodes[index.level()][index.index()];
	return NULL;
}

Node* ExtendableNode::child(NodeIdType id)
{
	Node* res = NULL;

	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i]->id() == id ) res = subnodes[level][i];

	return res;
}

Node* ExtendableNode::child(const QString& name)
{
	return get(name);
}

QString ExtendableNode::childReferenceName(const Node* child) const
{
	for (int level = 0; level < subnodes.size(); ++level)
	{
		int index = subnodes[level].indexOf(const_cast<Node*> (child)); // TODO find a way to do this cleanly
		if ( index >= 0 ) return meta.attribute(ExtendableIndex(level, index)).name();
	}

	return QString();
}

bool ExtendableNode::hasAttribute(const QString& attributeName)
{
	return meta.hasAttribute(attributeName);
}

QList< QPair<QString, Node*> > ExtendableNode::getAllAttributes(bool includeNullValues)
{
	QList< QPair<QString, Node*> > result;

	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes[level][i] || includeNullValues ) result.append(QPair<QString,Node*>((*currentLevel)[i].name(),subnodes[level][i]));
	}

	return result;
}

void ExtendableNode::removeOptional(const ExtendableIndex &attributeIndex)
{
	if ( meta.attribute(attributeIndex).optional() )
	{
		execute(new ExtendedNodeChild(this, subnodes[attributeIndex.level()][attributeIndex.index()], attributeIndex, &subnodes, false));
	}
	else
		throw ModelException("Trying to remove a non-optional attribute");
}

void ExtendableNode::save(PersistentStore &store) const
{
	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);
		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes[level][i] != NULL && currentLevel->at(i).persistent() ) store.saveNode(subnodes[level][i], currentLevel->at(i).name(), currentLevel->at(i).partialHint());
	}
}

void ExtendableNode::load(PersistentStore &store)
{
	removeAllNodes();

	QList<LoadedNode> children = store.loadAllSubNodes(this);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		ExtendableIndex index = meta.indexForAttribute(ln->name);
		if ( !index.isValid() ) throw ModelException("Node has attribute " + ln->name + " in persistent store, but this attribute is not registered");

		execute(new ExtendedNodeChild(this, ln->node, ExtendableIndex(index.level(),index.index()), &subnodes, true));
	}

	verifyHasAllMandatoryAttributes();
}

void ExtendableNode::removeAllNodes()
{
	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i] ) execute(new ExtendedNodeChild(this, subnodes[level][i], ExtendableIndex(level,i), &subnodes, false));
}

void ExtendableNode::verifyHasAllMandatoryAttributes()
{
	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes[level][i] == NULL && (*currentLevel)[i].optional() == false )
				throw ModelException("An ExtendableNode has an uninitialized mandatory attribute " + (*currentLevel)[i].name());
	}
}

}

