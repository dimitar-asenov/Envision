/***********************************************************************************************************************
 * Node.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Node.h"
#include <QMutexLocker>
#include "modelbase.h"
#include "Model.h"
#include "UndoCommand.h"

using namespace Logger;

namespace Model {

/***********************************************************************************************************************
 * STATIC MEMBERS
 **********************************************************************************************************************/
QMap<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QMap<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

/***********************************************************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 **********************************************************************************************************************/
Node::Node(Node* parent_, Model* model_) :
	parent(parent_), id(0), revision(0), fullyLoaded(true)
{
	Model *model = model_;
	if (model == NULL) model = getModel();
	else if (getModel() != NULL){}; // TODO here throw an exception. This should never be the case.

	id = model->generateNextId();
}

Node::Node(Node* parent_, NodeIdType id_) :
	parent(parent_), id(id_), revision(0), fullyLoaded(true)
{
}

Node::~Node()
{
	// TODO Node destructor: Does something need to go here?
}

/***********************************************************************************************************************
 * MAIN METHODS
 **********************************************************************************************************************/
void Node::loadFully(PersistentStore&)
{
}

void Node::execute(UndoCommand *command)
{
	getModel()->pushCommandOnUndoStack(command);
}

/***********************************************************************************************************************
 * GETTERS AND SETTERS
 **********************************************************************************************************************/
Model* Node::getModel()
{
	return Model::getModel(getRoot());
}

Node* Node::getRoot()
{
	if ( parent == NULL ) return this;

	return parent->getRoot();
}

Node* Node::getParent()
{
	return parent;
}

Node* Node::getChild(NodeIdType)
{
	return NULL;
}

NodeIdType Node::getId()
{
	return id;
}

bool Node::isNewPersistenceUnit()
{
	return false;
}

int Node::getRevision()
{
	return revision;
}

void Node::incrementRevision()
{
	revision++;
}

void Node::addToRevision(int valueToAdd)
{
	revision += valueToAdd;
}

bool Node::isFullyLoaded()
{
	return fullyLoaded;
}

QString Node::getReferenceName()
{
	return QString();
}
/***********************************************************************************************************************
 * STATIC METHODS
 **********************************************************************************************************************/
bool Node::registerNodeType(const QString &type, const NodeConstructor constructor, const NodePersistenceConstructor persistenceconstructor)
{
	if ( nodeConstructorRegister.contains(type) || nodePersistenceConstructorRegister.contains(type) ) return false;

	nodeConstructorRegister.insert(type, constructor);
	nodePersistenceConstructorRegister.insert(type, persistenceconstructor);

	ModelBase::log()->add(Log::LOGINFO, "Registered new node type " + type);

	return true;
}

Node* Node::createNewNode(const QString &type, Node* parent, Model* model)
{
	if ( nodeConstructorRegister.contains(type) )
	{
		return nodeConstructorRegister.value(type)(parent, model);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node. Requested node type has not been registered.");
		return NULL;
	}
}

Node* Node::createNewNode(const QString &type, Node* parent, NodeIdType id, PersistentStore &store, bool partialLoadHint)
{
	if ( nodePersistenceConstructorRegister.contains(type) )
	{
		return nodePersistenceConstructorRegister.value(type)(parent, id, store, partialLoadHint);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node from persistence. Requested node type has not been registered.");
		return NULL;
	}
}

}
