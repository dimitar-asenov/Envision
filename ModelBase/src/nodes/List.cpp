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
#include "commands/ListRemove.h"
#include "commands/ListPut.h"
#include "nodes/nodeMacros.h"

#include "Core/headers/global.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(List)

List::List(Node *parent, Model* model) :
	Node(parent, model)
{
}

List::List(Node *parent, NodeIdType id, PersistentStore &store, bool partialHint) :
	Node(parent, id)
{
	fullyLoaded = !partialHint;

	if ( fullyLoaded )
	{
		QList<LoadedNode> children = store.loadAllSubNodes(this);
		loadSubNodes(children);
	}

}

List::~List()
{
	for (int i = 0; i < nodes_.size(); ++i)
		SAFE_DELETE( nodes_[i] );
}

void List::loadSubNodes(QList<LoadedNode>& nodeList)
{
	for (QList<LoadedNode>::iterator ln = nodeList.begin(); ln != nodeList.end(); ln++)
	{
		bool ok = true;
		int index = ln->name.toInt(&ok);

		if ( !ok ) throw ModelException("Could not read the index of a list item. Index value is: " + ln->name);

		if ( index >= nodes_.size() ) nodes_.resize(index + 1);
		nodes_[index] = ln->node;
	}
}

void List::save(PersistentStore &store) const
{
	if ( fullyLoaded )
		for (int i = 0; i < nodes_.size(); ++i)
			store.saveNode(nodes_[i], QString::number(i), false);

	// TODO Document this somewhere useful. Like in the Persistent store interface.
	// If the node is partially loaded the Store will automatically fill in the missing fields by taking the old version
	// of every subnode whose name is not specified here.

}

void List::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load a List node from an incompatible node type " + store.currentNodeType());

	if (!fullyLoaded) loadFully(* (model()->store()));

	clear();

	QList<LoadedNode> children = store.loadAllSubNodes(this);
	for (QList<LoadedNode>::iterator ln = children.begin(); ln != children.end(); ln++)
	{
		bool ok = true;
		int index = ln->name.toInt(&ok);
		if ( !ok ) throw ModelException("Could not read the index of a list item. Index value is: " + ln->name);

		execute(new ListPut(this, nodes_, ln->node, index));
	}
}

void List::loadFully(PersistentStore &store)
{
	if ( !fullyLoaded )
	{
		QList<LoadedNode> subnodes = store.loadPartialNode(this);
		loadSubNodes(subnodes);
		fullyLoaded = true;
		model()->emitNodeFullyLoaded(this);
	}
}

int List::size()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	return nodes_.size();
}

int List::indexOf(const Node* item) const
{
	// TODO: is this a QT bug, this is fishy
	Node *i = const_cast<Node *> (item); // <--- Workaround, since the call below can't be made with item.
	return nodes_.indexOf(i);
}

void List::remove(int index)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	execute(new ListRemove(this, nodes_, index));
}

void List::remove(Node* instance)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	int index = nodes_.indexOf(instance);
	if ( index >= 0 ) remove(index);
}

void List::clear()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	while (!nodes_.isEmpty()) remove(0);
}

void List::paste(ClipboardStore& clipboard, int position)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	for (int i = 0; i<clipboard.numNodes(); ++i)
	{
		Node* newNode = clipboard.create(model(), this);
		execute(new ListInsert(this, nodes_, newNode, position+i));

		if (clipboard.hasNext() ) clipboard.next();
	}
}

}
