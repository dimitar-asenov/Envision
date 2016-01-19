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

#include "TreeManager.h"
#include "../ModelException.h"
#include "../commands/NodeOwningCommand.h"
#include "../commands/SetModificationTarget.h"
#include "../commands/NoteNodeChange.h"
#include "../nodes/Reference.h"
#include "../nodes/UsedLibrary.h"
#include "../nodes/NameText.h"

#include "Core/src/Profiler.h"

#include "AllTreeManagers.h"

namespace Model {

TreeManager::TreeManager(Node* root)
{
	commands.setUndoLimit(100);
	AllTreeManagers::instance().add(this);

	if (root) setRoot(root);
}

TreeManager::TreeManager(const QString& name, Node* root) : TreeManager{root}
{
	setName(name);
}

TreeManager::~TreeManager()
{
	AllTreeManagers::instance().remove(this);

	// TODO Make sure to persist and destroy the tree in a nice way.
	// TODO Emit a signal that this manager is no longer valid.
	SAFE_DELETE( root_ );
}

void TreeManager::beginModification(Node* modificationTarget, const QString &text)
{
	exclusiveAccess.lockForWrite();
	modificationInProgress = true;
	modificationText = text;
	modifiedTargets.clear();
	removedTargets_.clear();

	if ( modificationTarget ) changeModificationTarget(modificationTarget);
}

void TreeManager::changeModificationTarget(Node* modificationTarget)
{
	if ( !modificationInProgress )
		throw ModelException("Switching modification targets without calling TreeManager.beginModification() first");

	pushCommandOnUndoStack( new SetModificationTarget(currentModificationTarget, currentModificationLock,
			modifiedTargets, modificationTarget));
}

void TreeManager::endModification(bool tryResolvingReferences)
{
	if ( pushedNewCommandsOnTheStack )
	{
		if (tryResolvingReferences) Reference::resolvePending();

		pushCommandOnUndoStack(new SetModificationTarget(currentModificationTarget, currentModificationLock,
				modifiedTargets, nullptr));
		pushedNewCommandsOnTheStack = false;
		commands.endMacro();
	}

	performedUndoRedo = false;

	QSet<Node*> mt = modifiedTargets;
	QSet<Node*> dt = removedTargets_;
	modifiedTargets.clear();
	removedTargets_.clear();

	modificationInProgress = false;
	exclusiveAccess.unlock();

	Q_EMIT nodesModified(mt, dt);
}

void TreeManager::beginExclusiveRead()
{
	exclusiveAccess.lockForRead();
}

void TreeManager::endExclusiveRead()
{
	exclusiveAccess.unlock();
}

bool TreeManager::canBeModified(const Node* node) const
{
	// Check that we are in a modification block
	if ( !modificationInProgress ) return false;

	// Check that this node has as a parent the current modification target.
	const Node* n = node;
	while ( n != nullptr && n != currentModificationTarget )
		n = n->parent();
	if ( n == nullptr ) return false;

	// Check that the access lock for this node is the current modification lock.
	return node->accessLock() == currentModificationLock;
}

void TreeManager::pushCommandOnUndoStack(UndoCommand* command)
{
	if ( !modificationInProgress )
		throw ModelException("Changing the application tree without calling TreeManager.beginModification() first");
	if ( performedUndoRedo )
		throw ModelException("Trying to execute new commands after performing an Undo or a Redo operation.");

	if ( pushedNewCommandsOnTheStack == false )
	{
		pushedNewCommandsOnTheStack = true;
		commands.beginMacro(modificationText);
	}

	commands.push(command);
	commands.push(new NoteNodeChange{modifiedTargets, removedTargets_, command});
}

void TreeManager::undo()
{
	if ( !modificationInProgress )
		throw ModelException("Requesting an Undo without calling TreeManager.beginModification() first");
	if ( pushedNewCommandsOnTheStack )
		throw ModelException("Requesting an undo in the middle of a modification after executing new commands.");
	performedUndoRedo = true;

	commands.undo();
}

void TreeManager::redo()
{
	if ( !modificationInProgress )
		throw ModelException("Requesting a Redo without calling TreeManager.beginModification() first");
	if ( pushedNewCommandsOnTheStack )
		throw ModelException("Requesting a Redo in the middle of a modification after executing new commands.");
	performedUndoRedo = true;

	commands.redo();
}

bool TreeManager::isOwnedByUndoStack(const Node* node, const NodeOwningCommand* excludeCommand) const
{
	for (int i = 0; i<commands.count(); ++i)
	{
		auto parent = commands.command(i);
		Q_ASSERT(parent->childCount() > 0);
		for (int childIndex = 0; childIndex < parent->childCount(); ++childIndex)
			if (isOwnedByCommand(node, static_cast<const UndoCommand*>(parent->child(childIndex)), excludeCommand))
				return true;
	}
	return false;
}

bool TreeManager::isOwnedByCommand(const Node* node, const UndoCommand* cmd, const NodeOwningCommand* excludeCommand)
{
	if ( cmd->owned() == node && cmd!=excludeCommand) return true;
	for (int childIndex = 0; childIndex < cmd->childCount(); ++childIndex)
		if (isOwnedByCommand(node, static_cast<const UndoCommand*>(cmd->child(childIndex)), excludeCommand))
			return true;

	return false;
}

void TreeManager::save(PersistentStore* store)
{
	if (name_.isEmpty()) throw ModelException("Saving a tree without a name");

	if (root_)
	{
		if (store) store->saveTree(this, name_);
		else if (store_) store_->saveTree(this, name_);
		else throw ModelException("Saving tree '" + name_ + "' without specifying a persistent store");
	}

	if (store && !store_) store_ = store;
}

void TreeManager::load(PersistentStore* store, const QString& name, bool loadPartially)
{
	Q_ASSERT(!root_);

	if (name.isEmpty()) throw ModelException("Loading a tree without specifying a name");
	if (!store) throw ModelException("Loading tree '" + name + "' without specifying a persistent store");

	name_ = name;
	store_ = store;
	partiallyLoaded_ = loadPartially;

	Core::Profiler::startOnce(name == "jedit", "Loading jEdit", "load.prof");
	auto root = store->loadTree(this, name, loadPartially);
	for (auto lib : root->usedLibraries())
		lib->loadLibrary(store->clone());
	Core::Profiler::stop("Loading jEdit");

	Core::Profiler::startOnce(name == "java", "Resolving references", "resolve.prof");
	setRoot(root);
	Core::Profiler::stop("Resolving references");
}

void TreeManager::setRoot(Node* node)
{
	Q_ASSERT(!root_);
	Q_ASSERT(node);
	Q_ASSERT(!node->parent());

	commands.clear();
	root_ = node;

	root_->setRootManager(this);
	Reference::unresolveAll(root_);
	Reference::resolvePending();

	Q_EMIT rootNodeSet(root_);
}

void TreeManager::notifyNodeChange(Node* node)
{
	if (modificationInProgress) modifiedTargets.insert(node);
	else Q_EMIT nodesModified({node}, {});
}

void TreeManager::emitNameModified(NameText* node, const QString &oldName)
{
	QSet<QString> names;
	names << node->get() << oldName;
	Reference::unresolveNames(root_, names);

	Q_EMIT nameModified(node, oldName);
}

}
