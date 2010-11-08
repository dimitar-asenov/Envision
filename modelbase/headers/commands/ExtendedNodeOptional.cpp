/***********************************************************************************************************************
 * ExtendedNodeOptional.cpp
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ExtendedNodeOptional.h"

namespace Model {

ExtendedNodeOptional::ExtendedNodeOptional(Node* target, Node* attribute_, int attributeIndex_, QVector<Node*>* nodeAttributes_, bool created_) :
	UndoCommand(target, created_ ? "create optional node" : "remove optional node"), attribute(attribute_), attributeIndex(attributeIndex_), nodeAttributes(nodeAttributes_), created(created_)
{
}

ExtendedNodeOptional::~ExtendedNodeOptional()
{
	if ( created && isUndone() ) delete attribute;
	if ( !created && !isUndone() ) delete attribute;
}

void ExtendedNodeOptional::redo()
{
	if (created) (*nodeAttributes)[attributeIndex] = attribute;
	else (*nodeAttributes)[attributeIndex] = NULL;

	UndoCommand::redo();
}

void ExtendedNodeOptional::undo()
{
	if (created) (*nodeAttributes)[attributeIndex] = NULL;
	else (*nodeAttributes)[attributeIndex] = attribute;

	UndoCommand::undo();
}

}
