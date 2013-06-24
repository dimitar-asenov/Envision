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

#include "Node.h"
#include "../modelbase.h"
#include "../model/Model.h"
#include "../model/ModelManager.h"
#include "commands/UndoCommand.h"
#include "ModelException.h"
#include "Reference.h"

using namespace Logger;

namespace Model {

::Core::InitializationRegistry& nodeTypeInitializationRegistry();
DEFINE_TYPE_ID_BASE(Node, nodeTypeInitializationRegistry, "Node",)

/***********************************************************************************************************************
 * STATIC MEMBERS
 **********************************************************************************************************************/
int Node::numRegisteredTypes_ = 0;
QMap<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QMap<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

/***********************************************************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 **********************************************************************************************************************/
Node::Node(Node* parent) :
	fullyLoaded(true), parent_(parent), revision_(0)
{
	if (parent && !parent->isModifyable())
		throw ModelException("Trying to create a node with an non-modifiable parent.");
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

	if (m)
	{
		if ( !m->canBeModified(this) ) throw ModelException("Can not modify the current node.");
		m->pushCommandOnUndoStack(command);
	}
	else
	{
		command->redo();
		SAFE_DELETE(command);
	}
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

bool Node::isModifyable() const
{
	Model* m = model();

	return !m || m->canBeModified(this);
}

bool Node::replaceChild(Node*, Node*)
{
	return false;
}

QList<Node*> Node::findSymbols(const QRegExp& symbolExp, Node* source, FindSymbolMode mode, bool exhaustAllScopes)
{
	QList<Node*> res;

	// If exhaustAllScopes is true and there is a parent item, we should let the parent find this symbol definition
	// and add it to the result. This symbol should not report itself in that case.
	if (mode == SEARCH_UP)
	{
		if (definesSymbol() && symbolExp.exactMatch(symbolName()) && !(exhaustAllScopes && parent()))
			res << this;
		else if (parent_)
			res << parent_->findSymbols(symbolExp, source, mode, exhaustAllScopes);
	}

	return res;
}

void Node::beginModification(const QString &text)
{
	if (auto m = model())
		m->beginModification(this, text);
}

void Node::endModification()
{
	if (auto m = model()) m->endModification();
}

/***********************************************************************************************************************
 * GETTERS AND SETTERS
 **********************************************************************************************************************/
Model* Node::model() const
{
	return ModelManager::instance().find(root());
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

void Node::setParent(Node* parent)
{
	//TODO: is this operation efficient and even possible when performed on top level objects such as namespaces and
	// packages?

	auto mOld = model();
	auto mNew = parent ? parent->model() : nullptr;

	QList<Node*> queue;
	queue.append(this);

	// Transfer unresolved references from the old model to the new one
	while (!queue.isEmpty())
	{
		if (auto r = dynamic_cast<Reference*>(queue.first()))
		{
			if (!r->isResolved() )
			{
				if (mOld) mOld->removeUnresolvedReference(r);
				if (mNew) mNew->addUnresolvedReference(r);
			}
		}

		queue.append(queue.first()->children());
		queue.removeFirst();
	}

	parent_ = parent;
}

QList<Node*> Node::children()
{
	return QList<Node*>();
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

bool Node::isNewPersistenceUnit() const
{
	return false;
}

Node* Node::persistentUnitNode() const
{
	const Node* persistentUnitNode = this;
	const Node* prev = this;

	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
	{
		prev = persistentUnitNode;
		persistentUnitNode = persistentUnitNode->parent();
	}

	if (persistentUnitNode) return const_cast<Node*> (persistentUnitNode);
	else return const_cast<Node*> (prev);
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
	return numRegisteredTypes_; // Id 0 is reserved for Node
}

Node* Node::createNewNode(const QString &type, Node* parent)
{
	if ( isTypeRegistered(type) )
	{
		return nodeConstructorRegister.value(type)(parent);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node. Requested node type '"
				+ type +"' has not been registered.");
		return nullptr;
	}
}

Node* Node::createNewNode(const QString &type, Node* parent, PersistentStore &store, bool partialLoadHint)
{
	if ( isTypeRegistered(type) )
	{
		return nodePersistenceConstructorRegister.value(type)(parent, store, partialLoadHint);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node from persistence. Requested node type '"
				+ type + "' has not been registered.");
		return nullptr;
	}
}

bool Node::isTypeRegistered(const QString &type)
{
	return nodeConstructorRegister.contains(type);
}

}
