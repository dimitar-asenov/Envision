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

#include "Reference.h"
#include "UsedLibrary.h"
#include "../ModelBasePlugin.h"
#include "../ModelException.h"
#include "../commands/NodeOwningCommand.h"
#include "../commands/UndoCommand.h"
#include "../model/AllTreeManagers.h"
#include "../model/TreeManager.h"
#include "../util/ResolutionRequest.h"

#include "Core/src/AdapterManager.h"
#include "Logger/src/Log.h"

namespace Model {

DEFINE_TYPE_ID_BASE(Node, "Node", )

QHash<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QHash<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

QSet<const Node*>& Node::partiallyLoadedNodes()
{
	static QSet<const Node*> set;
	return set;
}

Node::Node(Node* parent) : parent_{parent}, manager_{parent ? parent->manager_ : nullptr}
{
	if (parent && !parent->isModifyable())
		throw ModelException{"Trying to create a node with a non-modifiable parent."};
}

Node::Node(const Node&) : Node{nullptr} {}

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

void Node::execute(UndoCommand *command)
{
	if ( this != command->target() )
		throw ModelException{"Command target differs from current node when executing commands"};

	TreeManager* m = manager();

	bool hasOwnTreeManager = m;

	// The if below is needed to make it possible to use nodes already attached in the tree, in the set method
	// of new nodes, which are not yet attached. Without it this command will not be recorded in the undo stack.
	if (auto owning = dynamic_cast<NodeOwningCommand*>(command))
	{
		auto managerOfInserted = owning->insertedNode() ?
			AllTreeManagers::instance().managerOfOwningUndoStack(owning->insertedNode()) : nullptr;
		if (!m) m = managerOfInserted;
		else Q_ASSERT(!managerOfInserted || m == managerOfInserted);
		auto managerOfRemoved = owning->removedNode() ?
			AllTreeManagers::instance().managerOfOwningUndoStack(owning->removedNode()) : nullptr;
		if (!m) m = managerOfRemoved;
		else Q_ASSERT(!managerOfRemoved || m == managerOfRemoved);
	}


	if (m)
	{
		if (hasOwnTreeManager && !m->canBeModified(this) ) throw ModelException{"Can not modify the current node."};
		m->pushCommandOnUndoStack(command);
	}
	else
	{
		command->redo();
		SAFE_DELETE(command);
	}
}

Node* Node::lowestCommonAncestor(Node* other, Node** thisParent, Node** otherParent)
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

	Q_ASSERT(thisParents.isEmpty() == otherParents.isEmpty());
	if (thisParent)
	{
		if (thisParents.isEmpty()) *thisParent = nullptr;
		else *thisParent = thisParents.first();
	}
	if (otherParent)
	{
		if (otherParents.isEmpty()) *otherParent = nullptr;
		else *otherParent = otherParents.first();
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

Node* Node::firstAncestorOfType(const SymbolMatcher& typeMatch) const
{
	auto typeOrSubtypeMatches = [typeMatch](Model::Node* node) {
		if (typeMatch.isFixedString())
			return node->isSubtypeOf(typeMatch.matchPattern());
		return typeMatch.matches(node->typeName());
	};
	auto p = parent();
	while (p && !typeOrSubtypeMatches(p)) p = p->parent();
	return p;
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

bool Node::findSymbols(std::unique_ptr<ResolutionRequest> request) const
{
	bool found{};

	if (request->direction() == SEARCH_HERE && !isTransparentForNameResolution())
	{
		if (symbolMatches(request->matcher(), request->symbolTypes()))
		{
			found = true;
			request->result().insert(const_cast<Node*>(this));
		}
	}
	else if (request->direction() == SEARCH_DOWN
				|| (request->direction() == SEARCH_HERE && isTransparentForNameResolution()))
		for (auto c : childrenInScope())
			found = c->findSymbols(request->clone(SEARCH_HERE, false)) || found;
	else if (request->direction() == SEARCH_UP || request->direction() == SEARCH_UP_ORDERED)
	{
		auto ignore = childToSubnode(request->source());
		for (auto c : childrenInScope())
		{
			// Optimize the search by skipping this scope, since we've already searched there
			if (c != ignore)
				found = c->findSymbols(request->clone(SEARCH_HERE, false) ) || found;
		}

		if ((request->exhaustAllScopes() || !found)
			 && symbolMatches(request->matcher(), request->symbolTypes()) && !isTransparentForNameResolution())
		{
			found = true;
			request->result().insert(const_cast<Node*>(this));
		}

		if ((request->exhaustAllScopes() || !found) && parent_)
			found = parent_->findSymbols(request->clone(SEARCH_UP)) || found;

		// Search in libraries. This is only valid for root nodes
		if ((request->exhaustAllScopes() || !found) && !parent_)
			for (const UsedLibrary* lib : usedLibraries())
			{
				auto libRoot = lib->libraryRoot();
				if (libRoot)
					found = libRoot->findSymbols(request->clone(libRoot, SEARCH_DOWN)) || found;
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
	return QList<Node*>{};
}

bool Node::definesSymbol() const
{
	return false;
}

bool Node::isTransparentForNameResolution() const
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

void Node::registerNodeType(const QString &type, const NodeConstructor constructor,
		const NodePersistenceConstructor persistenceconstructor)
{
	if ( isTypeRegistered(type) )
		throw ModelException{"Trying to register a node type that has already been registered: " + type};

	nodeConstructorRegister.insert(type, constructor);
	nodePersistenceConstructorRegister.insert(type, persistenceconstructor);

	ModelBasePlugin::log().info("Registered new node type " + type);
}

Node* Node::createNewNode(const QString &type, Node* parent)
{
	auto iter = nodeConstructorRegister.find(type);
	if ( iter != nodeConstructorRegister.end() )
		return iter.value()(parent);
	else
	{
		ModelBasePlugin::log().error("Could not create new node. Requested node type '"
				+ type +"' has not been registered.");
		return nullptr;
	}
}

Node* Node::createNewNode(const QString &type, Node* parent, PersistentStore &store, bool partialLoadHint)
{
	auto iter = nodePersistenceConstructorRegister.find(type);
	if ( iter != nodePersistenceConstructorRegister.end() )
		return iter.value()(parent, store, partialLoadHint);
	else
	{
		ModelBasePlugin::log().error("Could not create new node from persistence. Requested node type '"
				+ type + "' has not been registered.");
		return nullptr;
	}
}

bool Node::isTypeRegistered(const QString &type)
{
	return nodeConstructorRegister.contains(type);
}

}
