/***********************************************************************************************************************
 * List.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/List.h"
#include "ModelException.h"
#include "nodes/Text.h"
#include "commands/PointerFieldSet.h"
#include "commands/NameChange.h"
#include "commands/ListInsert.h"
#include "commands/ListRemove.h"
#include "nodes/Node.h"
#include "nodes/nodeMacros.h"

namespace Model {

const char* REFERENCE_NAME_NODE_ID = "ModelBaseListRefName";

NODE_DEFINE_TYPE_REGISTRATION_METHODS(List)

List::List(Node *parent, Model* model) :
	Node(parent, model), referenceName( NULL )
{

}

List::List(Node *parent, NodeIdType id, PersistentStore &store, bool partialHint) :
	Node(parent, id)
{
	fullyLoaded = !partialHint;

	if ( partialHint )
	{
		Node* n = store.loadSubNode(parent, REFERENCE_NAME_NODE_ID);
		if ( n )
		{
			Text* text = dynamic_cast<Text*> (n);
			if ( !text ) throw ModelException("Invalid List reference name specification in persistent store.");

			referenceName = text;
		}
	}
	else
	{
		QList<LoadedNode> children = store.loadAllSubNodes(this);
		loadSubNodes(children);
	}

}

List::~List()
{
	for (int i = 0; i < nodes.size(); ++i)
		delete nodes[i];
}

void List::loadSubNodes(QList<LoadedNode>& nodeList)
{
	for (QList<LoadedNode>::iterator ln = nodeList.begin(); ln != nodeList.end(); ln++)
	{
		if ( ln->name != REFERENCE_NAME_NODE_ID )
		{
			bool ok = true;
			int index = ln->name.toInt(&ok);

			if ( !ok ) throw ModelException("Could not read the index of a list item. Index value is: " + ln->name);

			if (index >= nodes.size()) nodes.resize(index + 1);
			nodes[index] = ln->node;
		}
	}
}

void List::save(PersistentStore &store) const
{
	if ( !fullyLoaded ) throw ModelException("Trying to save a partially loaded node.");

	if ( referenceName ) store.saveNode(referenceName, REFERENCE_NAME_NODE_ID, false);
	for (int i = 0; i < nodes.size(); ++i)
		store.saveNode(nodes[i], QString::number(i), nodes[i]->isFullyLoaded() == false);
}

void List::loadFully(PersistentStore &store)
{
	if ( !fullyLoaded )
	{
		QList < LoadedNode > nodes = store.loadPartialNode(this);
		loadSubNodes(nodes);
		fullyLoaded = true;
	}
}

Node* List::getChild(NodeIdType id) const
{
	if ( referenceName && referenceName->getId() == id ) return referenceName;

	for (int i = 0; i < nodes.size(); ++i)
		if (nodes[i]->getId() == id) return nodes[i];

	return NULL;
}

Node* List::getChild(const QString& name) const
{
	if (name.isEmpty()) return NULL;

	for (int i = 0; i < nodes.size(); ++i)
			if (nodes[i]->getReferenceName() == name) return nodes[i];

	return NULL;
}

QString List::getReferenceName() const
{
	if ( referenceName ) return referenceName->get();
	else
		return QString();
}

QString List::getChildReferenceName(const Node* child) const
{
	for (int i = 0; i < nodes.size(); ++i)
				if (nodes[i] == child) return nodes[i]->getReferenceName();

	return QString();
}

void List::setReferenceName(const QString &name)
{
	QString currentName;
	if (referenceName) currentName = referenceName->get();

	Text* t = static_cast<Text*> (Node::createNewNode("Text",this, getModel()));
	t->set(name);

	execute( new NameChange(this, currentName, name, new PointerFieldSet<Text>(this, referenceName, t) ) );
}

int List::size() const
{
	return nodes.size();
}

Node* List::first()
{
	if ( nodes.isEmpty() ) throw ModelException("Trying to access the first element of an empty list.");
	return nodes.first();
}

const Node* List::first() const
{
	if ( nodes.isEmpty() ) throw ModelException("Trying to access the first element of an empty list.");
	return nodes.first();
}

Node* List::last()
{
	if ( nodes.isEmpty() ) throw ModelException("Trying to access the last element of an empty list.");
	return nodes.last();
}

const Node* List::last() const
{
	if ( nodes.isEmpty() ) throw ModelException("Trying to access the last element of an empty list.");
	return nodes.last();
}

Node* List::operator[](int i)
{
	return nodes[i];
}

const Node* List::operator[](int i) const
{
	return nodes[i];
}

Node* List::append(const QString& type)
{
	return insert(type, nodes.size());
}

Node* List::prepend(const QString& type)
{
	return insert(type, 0);
}

Node* List::insert(const QString& type, int position)
{
	Node* newNode = Node::createNewNode(type,this,getModel());
	execute( new ListInsert(this, nodes, newNode, position) );
	return newNode;
}

void List::remove(int index)
{
	execute( new ListRemove(this, nodes, index) );
}

void List::remove(Node* instance)
{
	int index = nodes.indexOf(instance);
	if (index >= 0) remove(index);
}

}
