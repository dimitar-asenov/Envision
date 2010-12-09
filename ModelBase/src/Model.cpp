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

namespace Model {

QList<Model*> Model::loadedModels;

Model::Model() :
	root(NULL), currentModificationTarget(NULL), currentModificationLock(NULL), pushedNewCommandsOnTheStack(false), performedUndoRedo(false), modificationInProgress(false), nextId(0), lastUsedStore(NULL)
{
	commands.setUndoLimit(100);
	loadedModels.append(this);
}

Model::~Model()
{
	loadedModels.removeAll(this);

	// TODO Make sure to persist and destroy the tree in a nice way.
	// TODO Emit a signal that this model is no longer valid.
	delete root;
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

NodeReadWriteLock* Model::getRootLock()
{
	return &rootLock;
}

bool Model::canBeModified(const Node* node) const
{
	// Check that we are in a modification block
	if ( !modificationInProgress ) return false;

	// Check that this node has as a parent the current modification target.
	const Node* n = node;
	while ( n != NULL && n != currentModificationTarget )
		n = n->getParent();
	if ( n == NULL ) return false;

	// Check that the access lock for this node is the current modification lock.
	return node->getAccessLock() == currentModificationLock;
}

Node* Model::getRoot()
{
	return root;
}

QString Model::getName()
{
	return name;
}

void Model::setName(const QString& name_)
{
	name = name_;
}

NodeIdType Model::generateNextId()
{
	if ( root != NULL && !modificationInProgress ) throw ModelException("Creating a new node without calling Model.beginModification() first");

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

void Model::save(PersistentStore& store)
{
	if ( root ) store.saveNode(root, "root", false);
	lastUsedStore = &store;
}

void Model::load(PersistentStore& store, const QString& name_)
{
	name = name_;
	if ( root == NULL )
	{
		lastUsedStore = &store;
		commands.clear();
		root = store.loadRootNode(name);
		emit rootCreated(root);
	}
}

Model* Model::getModel(Node* root)
{
	for (QList<Model*>::iterator model = loadedModels.begin(); model != loadedModels.end(); model++)
	{
		if ( (*model)->getRoot() == root ) return *model;
	}

	return NULL;
}

Node* Model::createRoot(const QString &typeName)
{
	if ( root == NULL )
	{
		commands.clear();
		root = Node::createNewNode(typeName, NULL, this);
		emit rootCreated(root);
	}

	return root;
}

PersistentStore* Model::getLastUsedStore()
{
	return lastUsedStore;
}

void Model::emitNameModified(Node* node, const QString &oldName)
{
	emit nameModified(node, oldName);
}

void Model::emitNodeFullyLoaded(Node* node)
{
	emit nodeFullyLoaded(node);
}

void Model::emitNodePartiallyLoaded(Node* node)
{
	emit nodePartiallyLoaded(node);
}

}
