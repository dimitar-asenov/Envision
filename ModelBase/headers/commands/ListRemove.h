/***********************************************************************************************************************
 * ListRemove.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LISTREMOVE_H_
#define LISTREMOVE_H_

#include "UndoCommand.h"

namespace Model {

class ListRemove: public UndoCommand
{
	private:
		QVector<Node*>& nodes;
		Node* removedNode;
		int removePosition;

	public:
		ListRemove(Node *target, QVector<Node*>& nodes, int position);
		virtual ~ListRemove();

		virtual void redo();
		virtual void undo();
};

}

#endif /* LISTREMOVE_H_ */
