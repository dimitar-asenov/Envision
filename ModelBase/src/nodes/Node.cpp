/***********************************************************************************************************************
 * Node.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Node.h"
#include "modelbase.h"
#include "Model.h"
#include "commands/UndoCommand.h"
#include "ModelException.h"

#include <QtCore/QMutexLocker>
#include <QtCore/QList>

using namespace Logger;

namespace Model {

/***********************************************************************************************************************
 * STATIC MEMBERS
 **********************************************************************************************************************/
int Node::numRegisteredTypes = 0;
QMap<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QMap<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

/***********************************************************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 **********************************************************************************************************************/
Node::Node(Node* parent_, Model* model_) :
	parent(parent_), id(0), revision(0), fullyLoaded(true)
{
	Model *model = model_;
	if ( model == NULL ) model = getModel();
	else if ( getModel() != NULL ) throw ModelException("Constructing a new Node with an explicitly specified model and existing parent model");

	if ( model == NULL ) throw ModelException("Constructing a node without a valid model");

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
	if ( this != command->getTarget() ) throw ModelException("Command target differs from current node when executing commands");

	Model* model = getModel();

	if ( !model->canBeModified(this) ) throw ModelException("Can not modify the current node.");

	model->pushCommandOnUndoStack(command);
}

Node* Node::getLowestCommonAncestor(Node* other)
{
	QList<Node*> thisParents;
	QList<Node*> otherParents;

	// Get all parents of the current node
	Node* n = this;
	while ( n )
	{
		thisParents.prepend(n);
		n = n->getParent();
	}

	// Get all parents of the other node
	n = other;
	while ( n )
	{
		otherParents.prepend(n);
		n = n->getParent();
	}

	// Find the lowest common ancestor
	n = NULL;
	while ( thisParents.size() > 0 && otherParents.size() > 0 && thisParents.first() == otherParents.first() )
	{
		n = thisParents.first();

		thisParents.removeFirst();
		otherParents.removeFirst();
	}

	return n;
}
/***********************************************************************************************************************
 * GETTERS AND SETTERS
 **********************************************************************************************************************/
Model* Node::getModel() const
{
	return Model::getModel(getRoot());
}

Node* Node::getRoot() const
{
	if ( parent == NULL ) return const_cast<Node*> (this);

	return parent->getRoot();
}

Node* Node::getParent() const
{
	return parent;
}

Node* Node::getChild(NodeIdType)
{
	return NULL;
}

Node* Node::getChild(const QString&)
{
	return NULL;
}

NodeIdType Node::getId() const
{
	return id;
}

bool Node::isNewPersistenceUnit() const
{
	return false;
}

NodeIdType Node::persistentUnitId() const
{
	const Node* persistentUnitNode = this;
	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
		persistentUnitNode = persistentUnitNode->getParent();

	if (persistentUnitNode == NULL || persistentUnitNode->getParent() == NULL) return 0;
	else return persistentUnitNode->getId();
}

Node* Node::persistentUnitNode()
{
	Node* persistentUnitNode = this;
	Node* prev = this;

	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
	{
		prev = persistentUnitNode;
		persistentUnitNode = persistentUnitNode->getParent();
	}

	if (persistentUnitNode) return persistentUnitNode;
	else return prev;
}

int Node::getRevision() const
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

bool Node::isFullyLoaded() const
{
	return fullyLoaded;
}

QString Node::getReferenceName() const
{
	return QString();
}

QString Node::getChildReferenceName(const Node*) const
{
	return QString();
}

NodeReadWriteLock* Node::getAccessLock() const
{
	if ( parent ) return parent->getAccessLock();
	else
		return getModel()->getRootLock();
}
/***********************************************************************************************************************
 * STATIC METHODS
 **********************************************************************************************************************/
int Node::registerNodeType(const QString &type, const NodeConstructor constructor, const NodePersistenceConstructor persistenceconstructor)
{
	if ( isTypeRegistered(type) ) throw ModelException("Trying to register a node type that has already been registered: " + type);

	nodeConstructorRegister.insert(type, constructor);
	nodePersistenceConstructorRegister.insert(type, persistenceconstructor);

	ModelBase::log()->add(Log::LOGINFO, "Registered new node type " + type);

	++numRegisteredTypes;
	return numRegisteredTypes - 1;
}

Node* Node::createNewNode(const QString &type, Node* parent, Model* model)
{
	if ( isTypeRegistered(type) )
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
	if ( isTypeRegistered(type) )
	{
		return nodePersistenceConstructorRegister.value(type)(parent, id, store, partialLoadHint);
	}
	else
	{
		ModelBase::log()->add(Log::LOGERROR, "Could not create new node from persistence. Requested node type has not been registered.");
		return NULL;
	}
}

bool Node::isTypeRegistered(const QString &type)
{
	return nodeConstructorRegister.contains(type);
}

}
