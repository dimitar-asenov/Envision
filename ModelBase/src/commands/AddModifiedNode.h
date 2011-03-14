/***********************************************************************************************************************
 * AddModifiedNode.h
 *
 *  Created on: Mar 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ADDMODIFIEDNODE_H_
#define ADDMODIFIEDNODE_H_

#include "commands/UndoCommand.h"

namespace Model {

class AddModifiedNode: public UndoCommand
{
	private:
		QList<Node*>& modifiedTargets;
		Node* target;

	public:
		AddModifiedNode(QList<Node*>& modifiedTargets, Node* target);
		virtual void redo();
		virtual void undo();
};

}

#endif /* ADDMODIFIEDNODE_H_ */
