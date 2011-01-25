/***********************************************************************************************************************
 * UndoCommand.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/UndoCommand.h"
#include "nodes/Node.h"

namespace Model {

UndoCommand::UndoCommand(Node* target_, const QString & text) :
	QUndoCommand(text), target(target_), undone(false)
{

}

UndoCommand::~UndoCommand()
{
}

void UndoCommand::redo()
{
	// Increment the revision counter of this node and all parents
	Node* n = target;

	while ( n )
	{
		n->incrementRevision();
		n = n->parent();
	}

	// set command state to done
	undone = false;
}

void UndoCommand::undo()
{
	// Decrement the revision counter of this node and all parents
	Node* n = target;

	while ( n )
	{
		n->addToRevision(-1);
		n = n->parent();
	}

	// set command state to done
	undone = true;
}

bool UndoCommand::isUndone()
{
	return undone;
}

Node* UndoCommand::getTarget()
{
	return target;
}

}
