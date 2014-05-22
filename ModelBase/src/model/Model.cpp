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

#include "Model.h"
#include "../ModelException.h"
#include "../commands/NodeOwningCommand.h"
#include "../commands/SetModificationTarget.h"
#include "../commands/AddModifiedNode.h"
#include "../nodes/Reference.h"
#include "../nodes/UsedLibrary.h"
#include "../nodes/NameText.h"

#include "Core/src/Profiler.h"

namespace Model {

Model::Model(Node* root)
{
	commands.setUndoLimit(100);
	manager().add(this);

	if (root) setRoot(root);
}

Model::Model(const QString& name, Node* root) : Model{root}
{
	setName(name);
}

Model::~Model()
{
	manager().remove(this);

	// TODO Make sure to persist and destroy the tree in a nice way.
	// TODO Emit a signal that this model is no longer valid.
	SAFE_DELETE( root_ );
}

void Model::beginModification(Node* modificationTarget, const QString &text)
{
	exclusiveAccess.lockForWrite();
	modificationInProgress = true;
	modificationText = text;
	modifiedTargets.clear();

	if ( modificationTarget ) changeModificationTarget(modificationTarget);
}

void Model::changeModificationTarget(Node* modificationTarget)
{
	if ( !modificationInProgress )
		throw ModelException("Switching modification targets without calling Model.beginModification() first");

	pushCommandOnUndoStack( new SetModificationTarget(currentModificationTarget, currentModificationLock,
			modifiedTargets, modificationTarget));
}

void Model::endModification(bool resolveReferences)
{
	if ( pushedNewCommandsOnTheStack )
	{
		if (resolveReferences) Reference::resolvePending();

		pushCommandOnUndoStack(new SetModificationTarget(currentModificationTarget, currentModificationLock,
				modifiedTargets, nullptr));
		pushedNewCommandsOnTheStack = false;
		commands.endMacro();
	}

	performedUndoRedo = false;

	QSet<Node*> mt = modifiedTargets;
	modifiedTargets.clear();

	modificationInProgress = false;
	exclusiveAccess.unlock();

	emit nodesModified(mt);
}

void Model::beginExclusiveRead()
{
	exclusiveAccess.lockForRead();
}

void Model::endExclusiveRead()
{
	exclusiveAccess.unlock();
}

bool Model::canBeModified(const Node* node) const
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

void Model::pushCommandOnUndoStack(UndoCommand* command)
{
	if ( !modificationInProgress )
		throw ModelException("Changing the application tree without calling Model.beginModification() first");
	if ( performedUndoRedo )
		throw ModelException("Trying to execute new commands after performing an Undo or a Redo operation.");

	if ( pushedNewCommandsOnTheStack == false )
	{
		pushedNewCommandsOnTheStack = true;
		commands.beginMacro(modificationText);
	}

	commands.push(command);
	if (command->target()) commands.push(new AddModifiedNode(modifiedTargets, command->target()));
}

void Model::undo()
{
	if ( !modificationInProgress )
		throw ModelException("Requesting an Undo without calling Model.beginModification() first");
	if ( pushedNewCommandsOnTheStack )
		throw ModelException("Requesting an undo in the middle of a modification after executing new commands.");
	performedUndoRedo = true;

	commands.undo();
}

void Model::redo()
{
	if ( !modificationInProgress )
		throw ModelException("Requesting a Redo without calling Model.beginModification() first");
	if ( pushedNewCommandsOnTheStack )
		throw ModelException("Requesting a Redo in the middle of a modification after executing new commands.");
	performedUndoRedo = true;

	commands.redo();
}

bool Model::isOwnedByUndoStack(const Node* node, const NodeOwningCommand* excludeCommand) const
{
	for(int i = 0; i<commands.count(); ++i)
	{
		auto parent = commands.command(i);
		Q_ASSERT(parent->childCount() > 0);
		for(int childIndex = 0; childIndex < parent->childCount(); ++childIndex)
			if (isOwnedByCommand(node, static_cast<const UndoCommand*>(parent->child(childIndex)), excludeCommand))
				return true;
	}
	return false;
}

bool Model::isOwnedByCommand(const Node* node, const UndoCommand* cmd, const NodeOwningCommand* excludeCommand)
{
	if ( cmd->owned() == node && cmd!=excludeCommand) return true;
	for(int childIndex = 0; childIndex < cmd->childCount(); ++childIndex)
		if (isOwnedByCommand(node, static_cast<const UndoCommand*>(cmd->child(childIndex)), excludeCommand))
			return true;

	return false;
}

void Model::save(PersistentStore* store)
{
	if (name_.isEmpty()) throw ModelException("Saving a model without a name");

	if (root_)
	{
		if (store) store->saveModel(this, name_);
		else if (store_) store_->saveModel(this, name_);
		else throw ModelException("Saving model '" + name_ + "' without specifying a persistent store");
	}

	if (store && !store_) store_ = store;
}

void Model::load(PersistentStore* store, const QString& name, bool loadPartially)
{
	Q_ASSERT(!root_);

	if (name.isEmpty()) throw ModelException("Loading a model without specifying a name");
	if (!store) throw ModelException("Loading model '" + name + "' without specifying a persistent store");

	name_ = name;
	store_ = store;

	Core::Profiler::startOnce(name == "java", "Loading the Java library", "load.prof");
	auto root = store->loadModel(this, name, loadPartially);
	for(auto lib : root->usedLibraries())
		lib->loadLibraryModel(store->clone());
	Core::Profiler::stop("Loading the Java library");

	Core::Profiler::startOnce(name == "java", "Resolving references", "resolve.prof");
	setRoot(root);
	Core::Profiler::stop("Resolving references");
}

void Model::setRoot(Node* node)
{
	Q_ASSERT(!root_);
	Q_ASSERT(node);
	Q_ASSERT(!node->parent());

	commands.clear();
	root_ = node;

	root_->setRootModel(this);
	Reference::unresolveAll(root_);
	Reference::resolvePending();

	emit rootNodeSet(root_);
}

void Model::notifyNodeChange(Node* node)
{
	if (modificationInProgress) modifiedTargets.insert(node);
	else emit nodesModified(QSet<Node*>() << node);
}

void Model::emitNameModified(NameText* node, const QString &oldName)
{
	QSet<QString> names;
	names << node->get() << oldName;
	Reference::unresolveNames(root_, names);

	emit nameModified(node, oldName);
}

}
