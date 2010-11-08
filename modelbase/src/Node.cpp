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

using namespace Logger;

namespace Model {

/***********************************************************************************************************************
 * STATIC MEMBERS
 **********************************************************************************************************************/
NodeIdType Node::nextId = 0;
QMutex Node::nextIdAccess;
QMap<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QMap<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

/***********************************************************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 **********************************************************************************************************************/
Node::Node(Node* parent_) :
	parent(parent_), revision(0), fullyLoaded(true)
{
	QMutexLocker locker(&nextIdAccess);

	id = nextId;
	nextId++;
}

Node::Node(Node* parent_, NodeIdType id_) :
	parent(parent_), id(id_), revision(0), fullyLoaded(true)
{
}

Node::~Node()
{
	// TODO Auto-generated destructor stub
}

/***********************************************************************************************************************
 * MAIN METHODS
 **********************************************************************************************************************/
void Node::loadPartially(PersistentStore &store)
{
	loadFully(store);
}

void Node::execute(UndoCommand *command)
{
	Model::getModel( getRoot() )->pushCommandOnUndoStack(command);
}

/***********************************************************************************************************************
 * GETTERS AND SETTERS
 **********************************************************************************************************************/
Node* Node::getRoot()
{
	if (parent == NULL) return this;

	return parent->getRoot();
}

Node* Node::getParent()
{
	return parent;
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

Node* Node::createNewNode(const QString &type, Node* parent)
{
	if ( nodeConstructorRegister.contains(type) )
	{
		return nodeConstructorRegister.value(type)(parent);
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
