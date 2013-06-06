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

#include "nodes/composite/CompositeNode.h"
#include "../../commands/CompositeNodeChangeChild.h"

namespace Model {

::Core::InitializationRegistry& nodeTypeInitializationRegistry();
DEFINE_TYPE_ID_DERIVED(CompositeNode, nodeTypeInitializationRegistry, "CompositeNode",)

int CompositeNode::nextExtensionId_ = 0;

void CompositeNode::initType()
{
	typeIdVariable() = Node::registerNodeType("CompositeNode", ::Model::createNewNode< CompositeNode >,
			::Model::createNodeFromPersistence< CompositeNode >);
}

AttributeChain& CompositeNode::topLevelMeta()
{
	return meta;
}

CompositeNode::CompositeNode(Node *parent) :
	Super(parent), meta(CompositeNode::getMetaData())
{
	throw ModelException("Constructing an CompositeNode class directly, without specifying meta data");
}

CompositeNode::CompositeNode(Node *parent, PersistentStore &, bool) :
	Super(parent), meta(CompositeNode::getMetaData())
{
	throw ModelException("Constructing an CompositeNode class directly, without specifying meta data");
}

CompositeNode::CompositeNode(Node *parent, AttributeChain& metaData) :
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

CompositeNode::CompositeNode(Node *parent, PersistentStore &store, bool, AttributeChain& metaData) :
	Super(parent), meta(metaData), subnodes(meta.numLevels())
{
	for (int level = 0; level < meta.numLevels(); ++level)
		subnodes[level] = QVector<Node*> (meta.level(level)->size(), nullptr);

	QList<LoadedNode> children = store.loadAllSubNodes(this);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		CompositeIndex index = meta.indexForAttribute(ln->name);
		if ( !index.isValid() ) throw ModelException("Node has attribute "
				+ ln->name + " in persistent store, but this attribute is not registered");

		subnodes[index.level()][index.index()] = ln->node;
		ln->node->setParent(this);
	}

	verifyHasAllMandatoryAttributes();
}

CompositeNode::~CompositeNode()
{
	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i] ) SAFE_DELETE( subnodes[level][i] );
}

AttributeChain& CompositeNode::getMetaData()
{
	static AttributeChain descriptions{"CompositeNode"};
	return descriptions;
}

CompositeIndex CompositeNode::registerNewAttribute(AttributeChain& metaData, const QString &attributeName,
		const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent)
{
	return registerNewAttribute(metaData, Attribute(attributeName, attributeType,
												isOptional, canBePartiallyLoaded, isPersistent));
}

CompositeIndex CompositeNode::registerNewAttribute(AttributeChain& metaData, const Attribute& attribute)
{
	if ( metaData.hasAttribute(attribute.name()) )
		throw ModelException("Trying to register new attribute " + attribute.name() + " but this name already exists");

	metaData.append(attribute);

	return CompositeIndex(metaData.numLevels() - 1, metaData.size() - 1);
}

void CompositeNode::set(const CompositeIndex &attributeIndex, Node* node)
{
	Q_ASSERT( attributeIndex.isValid() );
	Q_ASSERT( attributeIndex.level() < subnodes.size());
	Q_ASSERT( attributeIndex.index() < subnodes[attributeIndex.level()].size());
	Q_ASSERT( node || meta.attribute(attributeIndex).optional());
	execute(new CompositeNodeChangeChild(this, node, attributeIndex, &subnodes));
}

Node* CompositeNode::get(const QString &attributeName) const
{
	CompositeIndex index = meta.indexForAttribute(attributeName);
	if ( index.isValid() ) return subnodes[index.level()][index.index()];
	return nullptr;
}

CompositeIndex CompositeNode::indexOf(Node* node) const
{
	if (node)
	{
		for (int level = 0; level < subnodes.size(); ++level)
			for (int i = 0; i < subnodes[level].size(); ++i)
				if (subnodes[level][i] == node)
					return CompositeIndex(level, i);
	}

	return CompositeIndex();
}

QList<Node*> CompositeNode::children()
{
	QList<Node*> result;
	for( auto p : getAllAttributes(false) )
		if (p.second != nullptr) result.append(p.second);

	return result;
}

bool CompositeNode::replaceChild(Node* child, Node* replacement)
{
	if (!child || !replacement) return false;

	CompositeIndex index = indexOf(child);
	if (!index.isValid()) return false;

	if ( !index.isValid() ) throw ModelException("Trying to set an attribute with an invalid Index");
	execute(new CompositeNodeChangeChild(this, replacement, index, &subnodes));
	return true;
}

bool CompositeNode::hasAttribute(const QString& attributeName)
{
	return meta.hasAttribute(attributeName);
}

QList< QPair<QString, Node*> > CompositeNode::getAllAttributes(bool includeNullValues)
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

void CompositeNode::removeOptional(const CompositeIndex &attributeIndex)
{
	if ( meta.attribute(attributeIndex).optional() )
	{
		execute(new CompositeNodeChangeChild(this, nullptr, attributeIndex, &subnodes));
	}
	else
		throw ModelException("Trying to remove a non-optional attribute");
}

void CompositeNode::save(PersistentStore &store) const
{
	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);
		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes[level][i] != nullptr && currentLevel->at(i).persistent() )
				store.saveNode(subnodes[level][i], currentLevel->at(i).name(), currentLevel->at(i).partialHint());
	}
}

void CompositeNode::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load a CompositeNode from an incompatible node type "
				+ store.currentNodeType());

	removeAllNodes();

	QList<LoadedNode> children = store.loadAllSubNodes(this);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		CompositeIndex index = meta.indexForAttribute(ln->name);
		if ( !index.isValid() )
			throw ModelException("Node has attribute "
					+ ln->name + " in persistent store, but this attribute is not registered");

		execute(new CompositeNodeChangeChild(this, ln->node, CompositeIndex(index.level(),index.index()), &subnodes));
	}

	verifyHasAllMandatoryAttributes();
}

void CompositeNode::removeAllNodes()
{
	for (int level = 0; level < subnodes.size(); ++level)
		for (int i = 0; i < subnodes[level].size(); ++i)
			if ( subnodes[level][i] )
				execute(new CompositeNodeChangeChild(this, nullptr, CompositeIndex(level,i), &subnodes));
}

void CompositeNode::verifyHasAllMandatoryAttributes()
{
	for (int level = 0; level < meta.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta.level(level);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes[level][i] == nullptr && (*currentLevel)[i].optional() == false )
				throw ModelException("An CompositeNode of type '" + meta.typeName()
						+ "' has an uninitialized mandatory attribute '"
						+ (*currentLevel)[i].name() +"'");
	}
}

}

