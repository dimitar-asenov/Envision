/***********************************************************************************************************************
 * Node.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Node.h"
#include <QMutexLocker>
#include "modelbase.h"

using namespace Logger;

namespace Model {

/***********************************************************************************************************************
 * STATIC MEMBERS
 **********************************************************************************************************************/
Node::IdType Node::nextId = 0;
QMutex Node::nextIdAccess;
QMap<QString, Node::NodeConstructor> Node::nodeTypeRegister;

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

Node::Node(Node* parent_, IdType id_) :
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

/***********************************************************************************************************************
 * GETTERS AND SETTERS
 **********************************************************************************************************************/
Node* Node::getParent()
{
	return parent;
}

Node::IdType Node::getId()
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
bool Node::registerNodeType(const QString &type, const NodeConstructor constructor)
{
	if ( nodeTypeRegister.contains(type) ) return false;

	nodeTypeRegister.insert(type, constructor);

	ModelBase::log()->add(Log::LOGINFO, "Registered new node type " + type);

	return true;
}

Node* Node::createNewNode(const QString &type, Node* parent)
{
	if ( nodeTypeRegister.contains(type) )
	{
		return nodeTypeRegister.value(type)(parent);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node. Requested node type has not been registered.");
		return NULL;
	}
}

}
