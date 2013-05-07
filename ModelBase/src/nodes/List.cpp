/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "nodes/List.h"
#include "ModelException.h"
#include "nodes/Text.h"
#include "commands/PointerFieldSet.h"
#include "commands/ListRemove.h"
#include "commands/ListPut.h"
#include "nodes/nodeMacros.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(List)

List::List(Node *parent) : Super(parent)
{
}

List::List(Node *parent, PersistentStore &store, bool partialHint) : Super(parent)
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
		ln->node->setParent(this);
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

QList<Node*> List::children()
{
	return nodes_.toList();
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

int List::indexOfSubitem(const Node* item) const
{
	for(int i = 0; i< nodes_.size(); ++i)
		if (nodes_.at(i) == item || nodes_.at(i)->isAncestorOf(item))
			return i;

	return -1;
}

void List::insert(int position, Node* node)
{
	if (!fullyLoaded) loadFully(* (model()->store()));
	execute(new ListInsert(this, nodes_, node, position));
}

void List::remove(int index)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	execute(new ListRemove(this, nodes_, index));
}

void List::remove(Node* instance )
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

bool List::replaceChild(Node* child, Node* replacement)
{
	if (!child || !replacement) return false;

	int index = indexOf(child);
	if (index < 0) return false;

	remove(index);
	insert(index, replacement);
	return true;
}

void List::paste(ClipboardStore& clipboard, int position)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	for (int i = 0; i<clipboard.numNodes(); ++i)
	{
		Node* newNode = clipboard.create(model(), nullptr); // We provide a null parent as this will be set in the instruction below.
		execute(new ListInsert(this, nodes_, newNode, position+i));

		if (clipboard.hasNext() ) clipboard.next();
	}
}

Node* List::findFirstSymbolDefinition(const QString& symbol, int beforeIndex)
{
	if (beforeIndex < 0) beforeIndex = nodes_.size();
	else if (beforeIndex > nodes_.size()) beforeIndex = nodes_.size();

	for(int i = 0; i<beforeIndex; ++i)
		if (nodes_[i]->definesSymbol() && nodes_[i]->symbolName() == symbol)
			return nodes_[i];

	return nullptr;
}

QList<Node*> List::findAllSymbolDefinitions(const QRegExp& symbolExp, int beforeIndex)
{
	QList<Node*> result;

	if (beforeIndex < 0) beforeIndex = nodes_.size();
	else if (beforeIndex > nodes_.size()) beforeIndex = nodes_.size();

	for(int i = 0; i<beforeIndex; ++i)
		if (nodes_[i]->definesSymbol() && symbolExp.exactMatch(nodes_[i]->symbolName()))
			result.append( nodes_[i] );

	return result;
}

Node* List::createDefaultElement()
{
	return nullptr;
}

}
