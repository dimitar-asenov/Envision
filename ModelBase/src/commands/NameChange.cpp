/***********************************************************************************************************************
 * NameChange.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/NameChange.h"
#include "nodes/Text.h"
#include "nodes/Node.h"
#include "Model.h"

namespace Model {

NameChange::NameChange(Node *target, const QString& oldName_, const QString& newName_, UndoCommand* command_) :
	UndoCommand(target, "Change name"), oldName(oldName_), newName(newName_), command(command_), text(NULL)
{
}

NameChange::NameChange(Node *target, Text* text_, const QString& newName_) :
		UndoCommand(target, "Change name"), oldName(text->get()), newName(newName_), command(NULL), text(text_)
{
}

NameChange::~NameChange()
{
	if (command) delete command;
}

void NameChange::redo()
{
	if (command) command->redo();
	if (text) text->text = newName;
	UndoCommand::getTarget()->getModel()->emitNameModified(UndoCommand::getTarget(), oldName);
}

void NameChange::undo()
{
	if (command) command->undo();
	if (text) text->text = oldName;
	UndoCommand::getTarget()->getModel()->emitNameModified(UndoCommand::getTarget(), newName);
}

}
