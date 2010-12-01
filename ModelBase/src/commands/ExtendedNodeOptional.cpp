/***********************************************************************************************************************
 * ExtendedNodeOptional.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/ExtendedNodeOptional.h"
#include "nodes/Node.h"

namespace Model {

ExtendedNodeOptional::ExtendedNodeOptional(Node* target, Node* attribute_, const ExtendableIndex &attributeIndex_, QVector< QVector<Node*> >* subnodes_, bool created_) :
	UndoCommand(target, created_ ? "create optional node" : "remove optional node"), attribute(attribute_), attributeIndex(attributeIndex_), subnodes(subnodes_), created(created_)
{
}

ExtendedNodeOptional::~ExtendedNodeOptional()
{
	if ( created && isUndone() ) delete attribute;
	if ( !created && !isUndone() ) delete attribute;
}

void ExtendedNodeOptional::redo()
{
	if (created) (*subnodes)[attributeIndex.level()][attributeIndex.index()] = attribute;
	else (*subnodes)[attributeIndex.level()][attributeIndex.index()] = NULL;

	UndoCommand::redo();
}

void ExtendedNodeOptional::undo()
{
	if (created) (*subnodes)[attributeIndex.level()][attributeIndex.index()] = NULL;
	else (*subnodes)[attributeIndex.level()][attributeIndex.index()] = attribute;

	UndoCommand::undo();
}

}
