/***********************************************************************************************************************
 * ExtendedNodeChild.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/ExtendedNodeChild.h"
#include "nodes/Node.h"

#include "Core/headers/global.h"

namespace Model {

ExtendedNodeChild::ExtendedNodeChild(Node* target, Node* newValue_, const ExtendableIndex &attributeIndex_, QVector< QVector<Node*> >* subnodes_) :
	UndoCommand(target, "set node"), newVal(newValue_), oldVal((*subnodes_)[attributeIndex_.level()][attributeIndex_.index()]),
	attributeIndex(attributeIndex_), subnodes(subnodes_)
{
}

ExtendedNodeChild::~ExtendedNodeChild()
{
	if ( isUndone() ) SAFE_DELETE(newVal);
	else SAFE_DELETE(oldVal);
}

void ExtendedNodeChild::redo()
{
	(*subnodes)[attributeIndex.level()][attributeIndex.index()] = newVal;

	UndoCommand::redo();
}

void ExtendedNodeChild::undo()
{
	(*subnodes)[attributeIndex.level()][attributeIndex.index()] = oldVal;

	UndoCommand::undo();
}

}
