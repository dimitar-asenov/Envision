/***********************************************************************************************************************
 * ListInsert.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/ListInsert.h"
#include "nodes/Node.h"

#include "Core/headers/global.h"

#include <QtCore/QVector>

namespace Model {

ListInsert::ListInsert(Node *target, QVector<Node*>& nodes_, Node* newNode_, int position) :
	UndoCommand(target, "insert node"), nodes(nodes_), newNode(newNode_), insertPosition(position), oldSize(nodes_.size())
{
}

ListInsert::~ListInsert()
{
	if ( isUndone() && newNode ) SAFE_DELETE(newNode);
}

void ListInsert::redo()
{
	if ( insertPosition >= nodes.size() ) nodes.resize(insertPosition + 1);
	nodes[insertPosition] = newNode;
	UndoCommand::redo();
}

void ListInsert::undo()
{
	nodes[insertPosition] = NULL;
	if (nodes.size() != oldSize) nodes.resize(oldSize);
	UndoCommand::undo();
}

}
