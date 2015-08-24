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

#include "Node.h"
#include "../ModelBasePlugin.h"
#include "../model/TreeManager.h"
#include "commands/UndoCommand.h"
#include "ModelException.h"
#include "Reference.h"
#include "Core/src/AdapterManager.h"
#include "UsedLibrary.h"
#include "../persistence/NodeIdMap.h"

using namespace Logger;

namespace Model {

DEFINE_TYPE_ID_BASE(Node, "Node", )

/***********************************************************************************************************************
 * STATIC MEMBERS
 **********************************************************************************************************************/
QHash<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QHash<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

QSet<const Node*>& Node::partiallyLoadedNodes()
{
	static QSet<const Node*> set;
	return set;
}

/***********************************************************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 **********************************************************************************************************************/
Node::Node(Node* parent) : parent_{parent}, manager_{parent ? parent->manager_ : nullptr}
{
	if (parent && !parent->isModifyable())
		throw ModelException("Trying to create a node with a non-modifiable parent.");
}

Node::Node(const Node&) : Node(nullptr) {}

Node::~Node()
{
	partiallyLoadedNodes().remove(this);

	for (auto m : AllTreeManagers::instance().loadedManagers())
		m->nodeIdMap().remove(this);
}

Node* Node::createDefaultInstance(Node*)
{
	Q_ASSERT(false);
	return nullptr;
}

/***********************************************************************************************************************
 * MAIN METHODS
 **********************************************************************************************************************/
void Node::execute(UndoCommand *command)
{
	if ( this != command->target() )
		throw ModelException("Command target differs from current node when executing commands");

	TreeManager* m = manager();

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

Node* Node::childToSubnode(const Node* other) const
{
	if (other == nullptr) return nullptr;

	const Node* child = other;
	const Node* parent = child->parent();

	while (parent && parent != this)
	{
		child = parent;
		parent = child->parent();
	}

	return parent == this ? const_cast<Node*>(child) : nullptr;
}

bool Node::isModifyable() const
{
	TreeManager* m = manager();

	return !m || m->canBeModified(this);
}

bool Node::replaceChild(Node*, Node*)
{
	return false;
}

bool Node::findSymbols(QSet<Node*>& result, const SymbolMatcher& matcher, const Node* source,
		FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const
{
	bool found{};

	if (direction == SEARCH_HERE)
	{
		if (symbolMatches(matcher, symbolTypes))
		{
			found = true;
			result.insert(const_cast<Node*>(this));
		}
	}
	else if (direction == SEARCH_DOWN)
	{
		for (auto c : childrenInScope())
			found = c->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;
	}
	else if (direction == SEARCH_UP)
	{
		auto ignore = childToSubnode(source);
		for (auto c : childrenInScope())
			if (c != ignore) // Optimize the search by skipping this scope, since we've already searched there
				found = c->findSymbols(result, matcher, source, SEARCH_HERE, symbolTypes, false) || found;

		if ((exhaustAllScopes || !found) && symbolMatches(matcher, symbolTypes))
		{
			found = true;
			result.insert(const_cast<Node*>(this));
		}

		if ((exhaustAllScopes || !found) && parent_)
			found = parent_->findSymbols(result, matcher, source, SEARCH_UP, symbolTypes, exhaustAllScopes) || found;

		// Search in libraries. This is only valid for root nodes
		if ((exhaustAllScopes || !found) && !parent_)
			for (auto lib : usedLibraries())
			{
				auto libRoot = lib->libraryRoot();
				if (libRoot)
					found = libRoot->findSymbols(result, matcher, libRoot, SEARCH_DOWN, symbolTypes, exhaustAllScopes)
							|| found;
			}
	}

	return found;
}

QList<Node*> Node::childrenInScope() const
{
	return children();
}

void Node::beginModification(const QString &text)
{
	if (auto m = manager())
		m->beginModification(this, text);
}

void Node::endModification()
{
	if (auto m = manager()) m->endModification();
}

QString Node::toDebugString()
{
	auto ntdsa = Core::AdapterManager::adapt<NodeToDebugStringAdapter>(this);

	QString ret = ntdsa ? ntdsa->str : "no debug string for node";
	SAFE_DELETE(ntdsa);
	return ret;
}

bool Node::hasPartiallyLoadedChildren() const
{
	if (isPartiallyLoaded()) return true;

	return false;
}
/***********************************************************************************************************************
 * GETTERS AND SETTERS
 **********************************************************************************************************************/
void Node::setParent(Node* parent)
{
	parent_ = parent;
	auto oldManager = manager_;
	manager_ = parent ? parent->manager_ : nullptr;
	if (oldManager != manager_)	propagateManagerToChildren();

	if (manager_) Reference::unresolveAfterNewSubTree(this);
}

void Node::propagateManagerToChildren()
{
	QList<Node*> stack;
	stack.append(children());
	while (!stack.isEmpty())
	{
		auto top = stack.takeLast();
		top->manager_ = manager_;
		stack.append(top->children());
	}
}

void Node::setRootManager(TreeManager* manager)
{
	Q_ASSERT(!parent_);
	Q_ASSERT(manager);
	manager_ = manager;
	propagateManagerToChildren();
}

QList<Node*> Node::children() const
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

NodeReadWriteLock* Node::accessLock() const
{
	if ( parent_ ) return parent_->accessLock();
	else
		return manager()->rootLock();
}

QList<const UsedLibrary*> Node::usedLibraries() const
{
	QList<const UsedLibrary*> all;
	for (auto c : children())
		all << c->usedLibraries();
	return all;
}

/***********************************************************************************************************************
 * STATIC METHODS
 **********************************************************************************************************************/
void Node::registerNodeType(const QString &type, const NodeConstructor constructor,
		const NodePersistenceConstructor persistenceconstructor)
{
	if ( isTypeRegistered(type) )
		throw ModelException("Trying to register a node type that has already been registered: " + type);

	nodeConstructorRegister.insert(type, constructor);
	nodePersistenceConstructorRegister.insert(type, persistenceconstructor);

	log.info("Registered new node type " + type);
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
		log.error("Could not create new node. Requested node type '"
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
		log.error("Could not create new node from persistence. Requested node type '"
				+ type + "' has not been registered.");
		return nullptr;
	}
}

bool Node::isTypeRegistered(const QString &type)
{
	return nodeConstructorRegister.contains(type);
}

__attribute__((optnone))
Node* Node::parentScope() const
{
	auto parent = this->parent();
	while (parent) {
		if (parent->symbolTable())
			return parent;
		parent = parent->parent();
	}
	return nullptr;
}

}
