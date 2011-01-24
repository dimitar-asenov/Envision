/***********************************************************************************************************************
 * ListPut.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/ListPut.h"
#include "nodes/Node.h"

#include "Core/headers/global.h"

#include <QtCore/QVector>

namespace Model {

ListPut::ListPut(Node *target, QVector<Node*>& nodes_, Node* newNode_, int position) :
	UndoCommand(target, "insert node"), nodes(nodes_), newNode(newNode_), putPosition(position), oldSize(nodes_.size())
{
}

ListPut::~ListPut()
{
	if ( isUndone() && newNode ) SAFE_DELETE(newNode);
}

void ListPut::redo()
{
	if ( putPosition >= nodes.size() ) nodes.resize(putPosition + 1);
	nodes[putPosition] = newNode;
	UndoCommand::redo();
}

void ListPut::undo()
{
	nodes[putPosition] = NULL;
	if (nodes.size() != oldSize) nodes.resize(oldSize);
	UndoCommand::undo();
}

}
