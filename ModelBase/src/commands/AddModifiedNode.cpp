/***********************************************************************************************************************
 * AddModifiedNode.cpp
 *
 *  Created on: Mar 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "AddModifiedNode.h"
#include "nodes/Node.h"

namespace Model {

AddModifiedNode::AddModifiedNode(QList<Node*>& modifiedTargets_, Node* target_) :
	UndoCommand(NULL, "Add a modification target"), modifiedTargets(modifiedTargets_), target(target_)
{
}

void AddModifiedNode::redo()
{
	if ( target && !modifiedTargets.contains(target) ) modifiedTargets.append(target);
	UndoCommand::redo();
}

void AddModifiedNode::undo()
{
	if ( target && !modifiedTargets.contains(target) ) modifiedTargets.append(target);


	UndoCommand::undo();
}

}
