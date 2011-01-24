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

ExtendedNodeChild::ExtendedNodeChild(Node* target, Node* attribute_, const ExtendableIndex &attributeIndex_, QVector< QVector<Node*> >* subnodes_, bool created_) :
	UndoCommand(target, created_ ? "create node" : "remove node"), attribute(attribute_), attributeIndex(attributeIndex_), subnodes(subnodes_), created(created_)
{
}

ExtendedNodeChild::~ExtendedNodeChild()
{
	if ( created && isUndone() ) SAFE_DELETE(attribute);
	if ( !created && !isUndone() ) SAFE_DELETE(attribute);
}

void ExtendedNodeChild::redo()
{
	if (created) (*subnodes)[attributeIndex.level()][attributeIndex.index()] = attribute;
	else (*subnodes)[attributeIndex.level()][attributeIndex.index()] = NULL;

	UndoCommand::redo();
}

void ExtendedNodeChild::undo()
{
	if (created) (*subnodes)[attributeIndex.level()][attributeIndex.index()] = NULL;
	else (*subnodes)[attributeIndex.level()][attributeIndex.index()] = attribute;

	UndoCommand::undo();
}

}
