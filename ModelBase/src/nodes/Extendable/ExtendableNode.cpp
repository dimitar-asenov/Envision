/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "nodes/Extendable/ExtendableNode.h"

namespace Model {

::Core::InitializationRegistry& nodeTypeInitializationRegistry();
DEFINE_TYPE_ID_DERIVED(ExtendableNode, nodeTypeInitializationRegistry, "ExtendableNode",)

int ExtendableNode::nextExtensionId_ = 0;

void ExtendableNode::initType()
{
	typeIdVariable() = Node::registerNodeType("ExtendableNode", ::Model::createNewNode< ExtendableNode >,
			::Model::createNodeFromPersistence< ExtendableNode >);
}

AttributeChain& ExtendableNode::topLevelMeta()
{
	return meta;
}

ExtendableNode::ExtendableNode(Node *parent) :
	Super(parent), meta(ExtendableNode::getMetaData())
{
	throw ModelException("Constructing an ExtendableNode class directly, without specifying meta data");
}

ExtendableNode::ExtendableNode(Node *parent, PersistentStore &, bool) :
	Super(parent), meta(ExtendableNode::getMetaData())
{
	throw ModelException("Constructing an ExtendableNode class directly, without specifying meta data");
}

ExtendableNode::ExtendableNode(Node *parent, AttributeChain& metaData) :
	Super(parent), meta(metaData), subnodes(meta.numLevels())
{
	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);
		subnodes[level] = QVector<Node*> (currentLevel->size(), nullptr);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( !(*currentLevel)[i].optional() )
				subnodes[level][i] = Node::createNewNode((*currentLevel)[i].type(), this);
	}
}

ExtendableNode::ExtendableNode(Node *parent, PersistentStore &store, bool, AttributeChain& metaData) :
	Super(parent), meta(metaData), subnodes(meta.numLevels())
{
	for (int level = 0; level < meta.numLevels(); ++level)
		subnodes[level] = QVector<Node*> (meta.level(level)->size(), nullptr);

	QList<LoadedNode> children = store.loadAllSubNodes(this);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		ExtendableIndex index = meta.indexForAttribute(ln->name);
		if ( !index.isValid() ) throw ModelException("Node has attribute "
				+ ln->name + " in persistent store, but this attribute is not registered");

		subnodes[index.level()][index.index()] = ln->node;
		ln->node->setParent(this);
	}

	verifyHasAllMandatoryAttributes();
}

ExtendableNode::~ExtendableNode()
{
	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i] ) SAFE_DELETE( subnodes[level][i] );
}

AttributeChain& ExtendableNode::getMetaData()
{
	static AttributeChain descriptions{"ExtendableNode"};
	return descriptions;
}

ExtendableIndex ExtendableNode::registerNewAttribute(AttributeChain& metaData, const QString &attributeName,
		const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent)
{
	return registerNewAttribute(metaData, Attribute(attributeName, attributeType,
												isOptional, canBePartiallyLoaded, isPersistent));
}

ExtendableIndex ExtendableNode::registerNewAttribute(AttributeChain& metaData, const Attribute& attribute)
{
	if ( metaData.hasAttribute(attribute.name()) )
		throw ModelException("Trying to register new attribute " + attribute.name() + " but this name already exists");

	metaData.append(attribute);

	return ExtendableIndex(metaData.numLevels() - 1, metaData.size() - 1);
}

void ExtendableNode::set(const ExtendableIndex &attributeIndex, Node* node)
{
	Q_ASSERT( attributeIndex.isValid() );
	Q_ASSERT( attributeIndex.level() < subnodes.size());
	Q_ASSERT( attributeIndex.index() < subnodes[attributeIndex.level()].size());
	execute(new ExtendedNodeChild(this, node, attributeIndex, &subnodes));
}

Node* ExtendableNode::get(const QString &attributeName) const
{
	ExtendableIndex index = meta.indexForAttribute(attributeName);
	if ( index.isValid() ) return subnodes[index.level()][index.index()];
	return nullptr;
}

ExtendableIndex ExtendableNode::indexOf(Node* node) const
{
	if (node)
	{
		for (int level = 0; level < subnodes.size(); ++level)
			for (int i = 0; i < subnodes[level].size(); ++i)
				if (subnodes[level][i] == node)
					return ExtendableIndex(level, i);
	}

	return ExtendableIndex();
}

QList<Node*> ExtendableNode::children()
{
	QList<Node*> result;
	for( auto p : getAllAttributes(false) )
		if (p.second != nullptr) result.append(p.second);

	return result;
}

bool ExtendableNode::replaceChild(Node* child, Node* replacement)
{
	if (!child || !replacement) return false;

	ExtendableIndex index = indexOf(child);
	if (!index.isValid()) return false;

	if ( !index.isValid() ) throw ModelException("Trying to set an attribute with an invalid Index");
	execute(new ExtendedNodeChild(this, replacement, index, &subnodes));
	return true;
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
			if ( subnodes[level][i] || includeNullValues )
				result.append(QPair<QString,Node*>((*currentLevel)[i].name(),subnodes[level][i]));
	}

	return result;
}

void ExtendableNode::removeOptional(const ExtendableIndex &attributeIndex)
{
	if ( meta.attribute(attributeIndex).optional() )
	{
		execute(new ExtendedNodeChild(this, nullptr, attributeIndex, &subnodes));
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
			if ( subnodes[level][i] != nullptr && currentLevel->at(i).persistent() )
				store.saveNode(subnodes[level][i], currentLevel->at(i).name(), currentLevel->at(i).partialHint());
	}
}

void ExtendableNode::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load an Extendable node from an incompatible node type "
				+ store.currentNodeType());

	removeAllNodes();

	QList<LoadedNode> children = store.loadAllSubNodes(this);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		ExtendableIndex index = meta.indexForAttribute(ln->name);
		if ( !index.isValid() )
			throw ModelException("Node has attribute "
					+ ln->name + " in persistent store, but this attribute is not registered");

		execute(new ExtendedNodeChild(this, ln->node, ExtendableIndex(index.level(),index.index()), &subnodes));
	}

	verifyHasAllMandatoryAttributes();
}

void ExtendableNode::removeAllNodes()
{
	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i] )
				execute(new ExtendedNodeChild(this, nullptr, ExtendableIndex(level,i), &subnodes));
}

void ExtendableNode::verifyHasAllMandatoryAttributes()
{
	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes[level][i] == nullptr && (*currentLevel)[i].optional() == false )
				throw ModelException("An ExtendableNode has an uninitialized mandatory attribute "
						+ (*currentLevel)[i].name());
	}
}

}

