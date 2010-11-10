/***********************************************************************************************************************
 * IntegerSet.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/IntegerSet.h"

namespace Model {

IntegerSet::IntegerSet(Node* target, int* field_, int setTo) :
	UndoCommand(target, "Set integer"), field(field_), oldValue(*field_), newValue(setTo)
{
}

void IntegerSet::redo()
{
	*field = newValue;

	UndoCommand::redo();
}

void IntegerSet::undo()
{
	*field = oldValue;

	UndoCommand::undo();
}

}
