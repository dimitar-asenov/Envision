/***********************************************************************************************************************
 * Model.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Model.h"
#include "UndoCommand.h"

namespace Model {

QList<Model*> Model::loadedModels;

Model::Model() :
	root(NULL), modificationInProgress(false), nextId(0)
{
	commands.setUndoLimit(100);
	loadedModels.append(this);
}

Model::~Model()
{
	loadedModels.removeAll(this);
	// TODO Make sure to persist and destroy the tree in a nice way.
}

void Model::beginModification(const QString &text)
{
	modification.lock();
	modificationInProgress = true;
	commands.beginMacro(text);
}

void Model::endModification()
{
	commands.endMacro();
	modificationInProgress = false;
	modification.unlock();
}

Node* Model::getRoot()
{
	return root;
}

NodeIdType Model::generateNextId()
{
	// TODO throw exception if the model is not under modification
	return nextId++;
}

void Model::pushCommandOnUndoStack(UndoCommand* command)
{
	commands.push(command);
}

void Model::undo()
{
	commands.undo();
}

void Model::redo()
{
	commands.redo();
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
	if (root == NULL )
	{
		commands.clear();
		root = Node::createNewNode(typeName, NULL, this);
	}

	return root;
}

}
