/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "CompositeNode.h"

#include "../../ModelException.h"
#include "../../commands/CompositeNodeChangeChild.h"
#include "../../persistence/PersistentStore.h"

namespace Model {

DEFINE_TYPE_ID_DERIVED(CompositeNode, "CompositeNode", )

bool CompositeNode::AUTOMATICALLY_CREATE_MISSING_REQUIRED_ATTRIBUTES_ON_LOAD_{true};

CompositeIndex CompositeNode::commentIndex =
		addAttributeToInitialRegistrationList_(commentIndex, "comment", "Node", false, true, true);

int CompositeNode::nextExtensionId_ = 0;

void CompositeNode::initType()
{
	Node::registerNodeType("CompositeNode",
			[](Node* parent) -> Node* { return CompositeNode::createDefaultInstance(parent);},
			[](Node *parent, PersistentStore &store, bool partialLoadHint) -> Node*
			{ return new CompositeNode{parent, store, partialLoadHint};});

	for (int i = 0; i<attributesToRegisterAtInitialization_().size(); ++i)
		attributesToRegisterAtInitialization_().at(i).first =
				registerNewAttribute(attributesToRegisterAtInitialization_().at(i).second);
}

CompositeNode* CompositeNode::createDefaultInstance( Node* parent)
{
	return new CompositeNode{parent};
}

AttributeChain& CompositeNode::topLevelMeta()
{
	return meta_;
}

CompositeNode::CompositeNode(Node *parent) :
	Super{parent}, meta_{CompositeNode::getMetaData()}
{
	throw ModelException{"Constructing an CompositeNode class directly, without specifying meta data"};
}

CompositeNode::CompositeNode(Node *parent, PersistentStore &, bool) :
	Super{parent}, meta_{CompositeNode::getMetaData()}
{
	throw ModelException{"Constructing an CompositeNode class directly, without specifying meta data"};
}

CompositeNode::CompositeNode(const CompositeNode& other)
	: Super{other}, meta_{other.meta_}, subnodes_{meta_.numLevels()}
{
	Q_ASSERT(subnodes_.size() == other.subnodes_.size());

	for (int level = 0; level < meta_.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta_.level(level);
		subnodes_[level] = QVector<Node*>{currentLevel->size(), nullptr};

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( auto node = other.subnodes_[level][i] )
			{
				auto cloned = node->clone();
				subnodes_[level][i] = cloned;
				cloned->setParent(this);
			}
	}
}

CompositeNode* CompositeNode::clone() const { return new CompositeNode{*this}; }

CompositeNode::CompositeNode(Node *parent, AttributeChain& metaData) :
	Super{parent}, meta_{metaData}, subnodes_{meta_.numLevels()}
{
	for (int level = 0; level < meta_.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta_.level(level);
		subnodes_[level] = QVector<Node*>{currentLevel->size(), nullptr};

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( !(*currentLevel)[i].optional() )
				subnodes_[level][i] = Node::createNewNode((*currentLevel)[i].type(), this);
	}
}

CompositeNode::CompositeNode(Node *parent, PersistentStore &store, bool, AttributeChain& metaData) :
	Super{parent}, meta_{metaData}, subnodes_{meta_.numLevels()}
{
	QSet<QString> partial;
	for (int level = 0; level < meta_.numLevels(); ++level)
	{
		subnodes_[level] = QVector<Node*>{meta_.level(level)->size(), nullptr};

		AttributeChain* currentLevel = meta_.level(level);
		for (int i = 0; i < currentLevel->size(); ++i)
			if ( (*currentLevel)[i].partial() ) partial.insert( (*currentLevel)[i].name() );
	}

	QList<LoadedNode> children = store.loadAllSubNodes(this, partial);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		CompositeIndex index = meta_.indexForAttribute(ln->name);
		if ( !index.isValid() ) throw ModelException{"Node has attribute "
				+ ln->name + " in persistent store, but this attribute is not registered"};

		auto attribute = meta_.attribute(index);
		Q_ASSERT(ln->node->isSubtypeOf(attribute.type()));

		// Skip loading partial optional children.
		if (!store.isLoadingPartially() || !attribute.optional() || !attribute.partial())
		{
			// No two child attributes may have the same name.
			Q_ASSERT(subnodes_[index.level()][index.index()] == nullptr);

			subnodes_[index.level()][index.index()] = ln->node;
			ln->node->setParent(this);
		}
	}

	checkOrCreateMandatoryAttributes(false);
}

CompositeNode::~CompositeNode()
{
	for (int level = 0; level < subnodes_.size(); ++level)
		for (int i = 0; i < subnodes_[level].size(); ++i)
			if ( subnodes_[level][i] ) SAFE_DELETE( subnodes_[level][i] );
}

AttributeChain& CompositeNode::getMetaData()
{
	static AttributeChain descriptions{"CompositeNode"};
	return descriptions;
}

CompositeIndex CompositeNode::registerNewAttribute(AttributeChain& metaData, const QString &attributeName,
		const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent)
{
	return registerNewAttribute(metaData, Attribute{attributeName, attributeType,
												isOptional, canBePartiallyLoaded, isPersistent});
}

CompositeIndex CompositeNode::registerNewAttribute(AttributeChain& metaData, const Attribute& attribute)
{
	if ( metaData.hasAttribute(attribute.name()) )
		throw ModelException{"Trying to register new attribute " + attribute.name() + " but this name already exists"};

	metaData.append(attribute);

	return CompositeIndex{metaData.numLevels() - 1, metaData.size() - 1};
}

CompositeIndex CompositeNode::registerNewAttribute(const Attribute& attribute)
{
	return registerNewAttribute(getMetaData(), attribute);
}

CompositeIndex CompositeNode::addAttributeToInitialRegistrationList_ (CompositeIndex& index,
	const QString &attributeName, const QString &attributeType, bool canBePartiallyLoaded, bool isOptional,
			 bool isPersistent)
{
	attributesToRegisterAtInitialization_().append(QPair< CompositeIndex&, Attribute>{index,
			Attribute{attributeName, attributeType, isOptional, canBePartiallyLoaded, isPersistent}});
	return CompositeIndex{};
}

QList<QPair< CompositeIndex&, Attribute> >& CompositeNode::attributesToRegisterAtInitialization_()
{
	static QList<QPair< CompositeIndex&, Attribute> > a;
	return a;
}

void CompositeNode::set(const CompositeIndex &attributeIndex, Node* node)
{
	Q_ASSERT( attributeIndex.isValid() );
	Q_ASSERT( attributeIndex.level() < subnodes_.size());
	Q_ASSERT( attributeIndex.index() < subnodes_[attributeIndex.level()].size());
	auto attribute = meta_.attribute(attributeIndex);
	Q_ASSERT( node || attribute.optional());
	Q_ASSERT( !node || node->isSubtypeOf(attribute.type()));
	execute(new CompositeNodeChangeChild{this, node, attributeIndex, &subnodes_});
}

Node* CompositeNode::get(const QString &attributeName) const
{
	CompositeIndex index = meta_.indexForAttribute(attributeName);
	if ( index.isValid() ) return subnodes_[index.level()][index.index()];
	return nullptr;
}

CompositeIndex CompositeNode::indexOf(Node* node) const
{
	if (node)
		for (int level = 0; level < subnodes_.size(); ++level)
			for (int i = 0; i < subnodes_[level].size(); ++i)
				if (subnodes_[level][i] == node)
					return CompositeIndex{level, i};

	return CompositeIndex{};
}


CompositeIndex CompositeNode::indexOf(const QString& nodeName) const
{
	return meta_.indexForAttribute(nodeName);
}

QList<Node*> CompositeNode::children() const
{
	QList<Node*> result;

	for (int level = 0; level < subnodes_.size(); ++level)
		for (int i = 0; i < subnodes_[level].size(); ++i)
			if (subnodes_[level][i]) result << subnodes_[level][i];

	return result;
}

bool CompositeNode::replaceChild(Node* child, Node* replacement)
{
	if (!child || !replacement) return false;

	CompositeIndex index = indexOf(child);
	Q_ASSERT(index.isValid());
	Q_ASSERT( !replacement || replacement->isSubtypeOf(meta_.attribute(index).type()));
	execute(new CompositeNodeChangeChild{this, replacement, index, &subnodes_});
	return true;
}

bool CompositeNode::hasAttribute(const QString& attributeName)
{
	return meta_.hasAttribute(attributeName);
}

QList< QPair<QString, Node*> > CompositeNode::getAllAttributes(bool includeNullValues)
{
	QList< QPair<QString, Node*> > result;

	for (int level = 0; level < meta_.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta_.level(level);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes_[level][i] || includeNullValues )
				result.append(QPair<QString, Node*>{(*currentLevel)[i].name(), subnodes_[level][i]});
	}

	return result;
}

void CompositeNode::save(PersistentStore &store) const
{
	for (int level = 0; level < meta_.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta_.level(level);
		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes_[level][i] != nullptr && currentLevel->at(i).persistent() )
				store.saveNode(subnodes_[level][i], currentLevel->at(i).name());
	}
}

void CompositeNode::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException{"Trying to load a CompositeNode from an incompatible node type "
				+ store.currentNodeType()};

	removeAllNodes();

	QSet<QString> partial;
	for (int level = 0; level < meta_.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta_.level(level);
		for (int i = 0; i < currentLevel->size(); ++i)
			if ( currentLevel->at(i).partial() )
				partial.insert(currentLevel->at(i).name());
	}

	QList<LoadedNode> children = store.loadAllSubNodes(this, partial);

	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		CompositeIndex index = meta_.indexForAttribute(ln->name);
		if ( !index.isValid() )
			throw ModelException{"Node has attribute "
					+ ln->name + " in persistent store, but this attribute is not registered"};

		auto attribute = meta_.attribute(index);
		Q_ASSERT(ln->node->isSubtypeOf(attribute.type()));

		// Skip loading partial optional children.
		if (!store.isLoadingPartially() || !attribute.optional() || !attribute.partial())
			execute(new CompositeNodeChangeChild{this, ln->node, {index.level(), index.index()}, &subnodes_});
	}

	checkOrCreateMandatoryAttributes(true);
}

void CompositeNode::removeAllNodes()
{
	for (int level = 0; level < subnodes_.size(); ++level)
		for (int i = 0; i < subnodes_[level].size(); ++i)
			if ( subnodes_[level][i] )
				execute(new CompositeNodeChangeChild{this, nullptr, CompositeIndex{level, i}, &subnodes_});
}

void CompositeNode::checkOrCreateMandatoryAttributes(bool useUndoableAction)
{
	for (int level = 0; level < meta_.numLevels(); ++level)
	{
		AttributeChain* currentLevel = meta_.level(level);

		for (int i = 0; i < currentLevel->size(); ++i)
			if ( subnodes_[level][i] == nullptr && (*currentLevel)[i].optional() == false )
			{
				auto nodeType = (*currentLevel)[i].type();
				if (AUTOMATICALLY_CREATE_MISSING_REQUIRED_ATTRIBUTES_ON_LOAD_ ||
					 nodeType.startsWith("TypedListOf") || nodeType.endsWith("List"))
				{
					auto newNode = Node::createNewNode(nodeType);
					if (useUndoableAction)
						execute(new CompositeNodeChangeChild{this, newNode, {level, i}, &subnodes_});
					else {
						subnodes_[level][i] = newNode;
						newNode->setParent(this);
					}
				}
				else
					throw ModelException{"An CompositeNode of type '" + meta_.typeName()
						+ "' has an uninitialized mandatory attribute '"
						+ (*currentLevel)[i].name() +"'"};
			}
	}
}

void CompositeNode::remove(const CompositeIndex &attributeIndex)
{
	Q_ASSERT(attributeIndex.isValid());

	if ( meta_.attribute(attributeIndex).optional() )
		execute(new CompositeNodeChangeChild{	this, nullptr, attributeIndex, &subnodes_});
	else
		execute(new CompositeNodeChangeChild{	this, Node::createNewNode(meta_.attribute(attributeIndex).type(), nullptr),
				attributeIndex, &subnodes_});
}

void CompositeNode::remove(Node* childNode)
{
	Q_ASSERT(childNode);
	remove(indexOf(childNode));
}

void CompositeNode::remove(QString childNodeName)
{
	remove(indexOf(childNodeName));
}

Node* CompositeNode::setDefault(QString nodeName)
{
	auto attributeIndex = indexOf(nodeName);

	auto newNode = Node::createNewNode(meta_.attribute(attributeIndex).type());
	set(attributeIndex, newNode);

	return newNode;
}

}
