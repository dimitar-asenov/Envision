/***********************************************************************************************************************
 * TextSet.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/TextSet.h"

namespace Model {

TextSet::TextSet(Node* target, QString* field_, QString setTo) :
	UndoCommand(target, "Set text"), field(field_), oldText(*field_), newText(setTo)
{
}

void TextSet::redo()
{
	*field = newText;

	UndoCommand::redo();
}

void TextSet::undo()
{
	*field = oldText;

	UndoCommand::undo();
}

}
