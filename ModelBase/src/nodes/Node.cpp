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
QHash<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QHash<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

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

Node* Node::createDefaultInstance(Node*)
{
	Q_ASSERT(false);
	return nullptr;
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

QSet<Node*> Node::findSymbols(const SymbolMatcher& matcher, Node* source, FindSymbolDirection direction,
		SymbolTypes symbolTypes, bool exhaustAllScopes)
{
	QSet<Node*> res;

	if (direction == SEARCH_HERE)
	{
		if (symbolMatches(matcher, symbolTypes))  res.insert(this);
	}
	else if (direction == SEARCH_DOWN)
	{
		for (auto c : childrenInScope())
			res.unite(c->findSymbols(matcher, source, SEARCH_HERE, symbolTypes, false));
	}
	else if (direction == SEARCH_UP)
	{
		for (auto c : childrenInScope())
			if (!c->isAncestorOf(source)) // Optimize the search by skipping this scope, since we've already searched there
				res.unite(c->findSymbols(matcher, source, SEARCH_HERE, symbolTypes, false));

		if ((exhaustAllScopes || res.isEmpty()) && symbolMatches(matcher, symbolTypes))
			res.insert(this);

		if ((exhaustAllScopes || res.isEmpty()) && parent_)
			res.unite(parent_->findSymbols(matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes));
	}

	return res;
}

QList<Node*> Node::childrenInScope()
{
	return children();
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
void Node::setParent(Node* parent)
{
	//TODO: is this operation efficient and even possible when performed on top level objects such as namespaces and
	// packages?

	auto mOld = model();
	auto mNew = parent ? parent->model() : nullptr;

	if (mOld || mNew)
	{
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

Node::SymbolTypes Node::symbolType() const
{
	return UNSPECIFIED;
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
	auto iter = nodeConstructorRegister.find(type);
	if ( iter != nodeConstructorRegister.end() )
	{
		return iter.value()(parent);
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
	auto iter = nodePersistenceConstructorRegister.find(type);
	if ( iter != nodePersistenceConstructorRegister.end() )
	{
		return iter.value()(parent, store, partialLoadHint);
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
