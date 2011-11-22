/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * Node.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Node.h"
#include "modelbase.h"
#include "Model.h"
#include "commands/UndoCommand.h"
#include "ModelException.h"

#include <QtCore/QMutexLocker>
#include <QtCore/QList>

using namespace Logger;

namespace Model {

/***********************************************************************************************************************
 * STATIC MEMBERS
 **********************************************************************************************************************/
int Node::numRegisteredTypes_ = 0;
QMap<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QMap<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

/***********************************************************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 **********************************************************************************************************************/
Node::Node(Node* parent, Model* argModel) :
	fullyLoaded(true), parent_(parent), id_(0), revision_(0)
{
	Model* ownModel = model();

	if (ownModel && argModel)
		throw ModelException("Constructing a new Node with an explicitly specified model and existing parent model");

	if (!ownModel && !argModel) throw ModelException("Constructing a node without a valid model");

	if (ownModel) id_ = ownModel->generateNextId();
	else id_ = argModel->generateNextId();
}

Node::Node(Node* parent, NodeIdType id) :
	fullyLoaded(true), parent_(parent), id_(id), revision_(0)
{
}

Node::~Node()
{
}

/***********************************************************************************************************************
 * MAIN METHODS
 **********************************************************************************************************************/
void Node::loadFully(PersistentStore&)
{
}

void Node::execute(UndoCommand *command)
{
	if ( this != command->target() ) throw ModelException("Command target differs from current node when executing commands");

	Model* m = model();

	if ( !m->canBeModified(this) ) throw ModelException("Can not modify the current node.");

	m->pushCommandOnUndoStack(command);
}

Node* Node::lowestCommonAncestor(Node* other)
{
	QList<Node*> thisParents;
	QList<Node*> otherParents;

	// Get all parents of the current node
	Node* n = this;
	while ( n )
	{
		thisParents.prepend(n);
		n = n->parent();
	}

	// Get all parents of the other node
	n = other;
	while ( n )
	{
		otherParents.prepend(n);
		n = n->parent();
	}

	// Find the lowest common ancestor
	n = nullptr;
	while ( thisParents.size() > 0 && otherParents.size() > 0 && thisParents.first() == otherParents.first() )
	{
		n = thisParents.first();

		thisParents.removeFirst();
		otherParents.removeFirst();
	}

	return n;
}

bool Node::isAncestorOf(const Node* other) const
{
	if (other == nullptr) return false;

	const Node* p = other->parent();

	while (p && p != this) p = p->parent();

	return p == this;
}

Node* Node::navigateTo(Node* source, QString path)
{
	if (source == this || isAncestorOf(source))
	{
		if (parent_) return parent_->navigateTo(this, path);
		else return nullptr;
	}
	else return nullptr;
}

/***********************************************************************************************************************
 * GETTERS AND SETTERS
 **********************************************************************************************************************/
Model* Node::model() const
{
	return Model::findModel(root());
}

Node* Node::root() const
{
	const Node* root = this;
	while (root->parent()) root = root->parent();
	return const_cast<Node*> (root);
}

Node* Node::parent() const
{
	return parent_;
}

bool Node::definesSymbol() const
{
	return false;
}

const QString& Node::symbolName() const
{
	static QString nullString;
	return nullString;
}

NodeIdType Node::id() const
{
	return id_;
}

bool Node::isNewPersistenceUnit() const
{
	return false;
}

NodeIdType Node::persistentUnitId() const
{
	const Node* persistentUnitNode = this;
	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
		persistentUnitNode = persistentUnitNode->parent();

	if (persistentUnitNode == nullptr || persistentUnitNode->parent() == nullptr) return 0;
	else return persistentUnitNode->id();
}

Node* Node::persistentUnitNode()
{
	Node* persistentUnitNode = this;
	Node* prev = this;

	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
	{
		prev = persistentUnitNode;
		persistentUnitNode = persistentUnitNode->parent();
	}

	if (persistentUnitNode) return persistentUnitNode;
	else return prev;
}

int Node::revision() const
{
	return revision_;
}

void Node::incrementRevision()
{
	revision_++;
}

void Node::addToRevision(int valueToAdd)
{
	revision_ += valueToAdd;
}

bool Node::isFullyLoaded() const
{
	return fullyLoaded;
}

NodeReadWriteLock* Node::accessLock() const
{
	if ( parent_ ) return parent_->accessLock();
	else
		return model()->rootLock();
}
/***********************************************************************************************************************
 * STATIC METHODS
 **********************************************************************************************************************/
int Node::registerNodeType(const QString &type, const NodeConstructor constructor, const NodePersistenceConstructor persistenceconstructor)
{
	if ( isTypeRegistered(type) ) throw ModelException("Trying to register a node type that has already been registered: " + type);

	nodeConstructorRegister.insert(type, constructor);
	nodePersistenceConstructorRegister.insert(type, persistenceconstructor);

	ModelBase::log()->add(Log::LOGINFO, "Registered new node type " + type);

	++numRegisteredTypes_;
	return numRegisteredTypes_ - 1;
}

Node* Node::createNewNode(const QString &type, Node* parent, Model* model)
{
	if ( isTypeRegistered(type) )
	{
		return nodeConstructorRegister.value(type)(parent, model);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node. Requested node type has not been registered.");
		return nullptr;
	}
}

Node* Node::createNewNode(const QString &type, Node* parent, NodeIdType id, PersistentStore &store, bool partialLoadHint)
{
	if ( isTypeRegistered(type) )
	{
		return nodePersistenceConstructorRegister.value(type)(parent, id, store, partialLoadHint);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node from persistence. Requested node type has not been registered.");
		return nullptr;
	}
}

bool Node::isTypeRegistered(const QString &type)
{
	return nodeConstructorRegister.contains(type);
}

}
