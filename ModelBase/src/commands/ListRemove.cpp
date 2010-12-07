/***********************************************************************************************************************
 * ListRemove.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/ListRemove.h"
#include "nodes/Node.h"

#include <QtCore/QVector>

namespace Model {

ListRemove::ListRemove(Node *target, QVector<Node*>& nodes_, int position) :
	UndoCommand(target, "remove node"), nodes(nodes_), removedNode(nodes_[position]), removePosition(position)
{
}

ListRemove::~ListRemove()
{
	if ( !isUndone() && removedNode ) delete removedNode;
}

void ListRemove::redo()
{
	nodes.remove(removePosition);
	UndoCommand::redo();
}

void ListRemove::undo()
{
	nodes.insert(removePosition, removedNode);
	UndoCommand::undo();
}

}
