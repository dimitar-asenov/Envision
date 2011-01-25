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
int Node::numRegisteredTypes_ = 0;
QMap<QString, Node::NodeConstructor> Node::nodeConstructorRegister;
QMap<QString, Node::NodePersistenceConstructor> Node::nodePersistenceConstructorRegister;

/***********************************************************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 **********************************************************************************************************************/
Node::Node(Node* parent, Model* argModel) :
	parent_(parent), id_(0), revision_(0), fullyLoaded(true)
{
	Model* ownModel = model();

	if (ownModel && argModel)
		throw ModelException("Constructing a new Node with an explicitly specified model and existing parent model");

	if (!ownModel && !argModel) throw ModelException("Constructing a node without a valid model");

	if (ownModel) id_ = ownModel->generateNextId();
	else id_ = argModel->generateNextId();
}

Node::Node(Node* parent, NodeIdType id) :
	parent_(parent), id_(id), revision_(0), fullyLoaded(true)
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

	Model* m = model();

	if ( !m->canBeModified(this) ) throw ModelException("Can not modify the current node.");

	m->pushCommandOnUndoStack(command);
}

Node* Node::lowestCommonAncestor(Node* other)
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
Model* Node::model() const
{
	return Model::findModel(root());
}

Node* Node::root() const
{
	const Node* root = this;
	while (root->parent()) root = root->parent();
	return const_cast<Node*> (root);
}

Node* Node::parent() const
{
	return parent_;
}

Node* Node::child(NodeIdType)
{
	return NULL;
}

Node* Node::child(const QString&)
{
	return NULL;
}

NodeIdType Node::id() const
{
	return id_;
}

bool Node::isNewPersistenceUnit() const
{
	return false;
}

NodeIdType Node::persistentUnitId() const
{
	const Node* persistentUnitNode = this;
	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
		persistentUnitNode = persistentUnitNode->parent();

	if (persistentUnitNode == NULL || persistentUnitNode->parent() == NULL) return 0;
	else return persistentUnitNode->id();
}

Node* Node::persistentUnitNode()
{
	Node* persistentUnitNode = this;
	Node* prev = this;

	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
	{
		prev = persistentUnitNode;
		persistentUnitNode = persistentUnitNode->parent();
	}

	if (persistentUnitNode) return persistentUnitNode;
	else return prev;
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

bool Node::isFullyLoaded() const
{
	return fullyLoaded;
}

QString Node::referenceName() const
{
	return QString();
}

QString Node::childReferenceName(const Node*) const
{
	return QString();
}

NodeReadWriteLock* Node::accessLock() const
{
	if ( parent_ ) return parent_->accessLock();
	else
		return model()->rootLock();
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

	++numRegisteredTypes_;
	return numRegisteredTypes_ - 1;
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
