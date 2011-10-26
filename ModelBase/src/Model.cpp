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
 * Model.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Model.h"
#include "ModelException.h"
#include "commands/UndoCommand.h"
#include "commands/SetModificationTarget.h"
#include "commands/AddModifiedNode.h"

#include "Core/headers/global.h"

#include <QtCore/QMetaType>

namespace Model {

QList<Model*> Model::loadedModels;

void Model::init()
{
	qRegisterMetaType< QList<Node*> >("QList<Node*>");
}

Model::Model() :
	root_(NULL), currentModificationTarget(NULL), currentModificationLock(NULL), pushedNewCommandsOnTheStack(false), performedUndoRedo(false), modificationInProgress(false), nextId(0), store_(NULL)
{
	commands.setUndoLimit(100);
	loadedModels.append(this);
}

Model::~Model()
{
	loadedModels.removeAll(this);

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
	if ( !modificationInProgress ) throw ModelException("Switching modification targets without calling Model.beginModification() first");

	pushCommandOnUndoStack(new SetModificationTarget(currentModificationTarget, currentModificationLock, modifiedTargets, modificationTarget));
}

void Model::endModification()
{
	if ( pushedNewCommandsOnTheStack )
	{
		pushCommandOnUndoStack(new SetModificationTarget(currentModificationTarget, currentModificationLock, modifiedTargets, NULL));
		pushedNewCommandsOnTheStack = false;
		commands.endMacro();
	}

	performedUndoRedo = false;

	QList<Node*> mt = modifiedTargets;
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
	while ( n != NULL && n != currentModificationTarget )
		n = n->parent();
	if ( n == NULL ) return false;

	// Check that the access lock for this node is the current modification lock.
	return node->accessLock() == currentModificationLock;
}

NodeIdType Model::generateNextId()
{
	if ( root_ != NULL && !modificationInProgress ) throw ModelException("Creating a new node without calling Model.beginModification() first");

	return nextId++;
}

void Model::pushCommandOnUndoStack(UndoCommand* command)
{
	if ( !modificationInProgress ) throw ModelException("Changing the application tree without calling Model.beginModification() first");
	if ( performedUndoRedo ) throw ModelException("Trying to execute new commands after performing an Undo or a Redo operation.");

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
	if ( !modificationInProgress ) throw ModelException("Requesting an Undo without calling Model.beginModification() first");
	if ( pushedNewCommandsOnTheStack ) throw ModelException("Requesting an undo in the middle of a modification after executing new commands.");
	performedUndoRedo = true;

	commands.undo();
}

void Model::redo()
{
	if ( !modificationInProgress ) throw ModelException("Requesting a Redo without calling Model.beginModification() first");
	if ( pushedNewCommandsOnTheStack ) throw ModelException("Requesting a Redo in the middle of a modification after executing new commands.");
	performedUndoRedo = true;

	commands.redo();
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

void Model::load(PersistentStore* store, const QString& name)
{
	if (name.isEmpty()) throw ModelException("Loading a model without specifying a name");
	if (!store) throw ModelException("Loading model '" + name + "' without specifying a persistent store");

	name_ = name;
	if ( root_ == NULL )
	{
		store_ = store;
		commands.clear();
		root_ = store_->loadModel(this, name_);
		emit rootCreated(root_);
	}
}

Model* Model::findModel(Node* root)
{
	for (QList<Model*>::iterator model = loadedModels.begin(); model != loadedModels.end(); model++)
	{
		if ( (*model)->root() == root ) return *model;
	}

	return NULL;
}

Node* Model::createRoot(const QString &typeName)
{
	if ( root_ == NULL )
	{
		commands.clear();
		root_ = Node::createNewNode(typeName, NULL, this);
		emit rootCreated(root_);
	}

	return root_;
}

}
