/***********************************************************************************************************************
 * ListInsert.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LISTINSERT_H_
#define LISTINSERT_H_

#include "UndoCommand.h"

namespace Model {

class ListInsert: public UndoCommand
{
	private:
		QVector<Node*>& nodes;
		Node* newNode;
		int insertPosition;

	public:
		ListInsert(Node *target, QVector<Node*>& nodes, Node* newNode, int position);
		virtual ~ListInsert();

		virtual void redo();
		virtual void undo();
};

}

#endif /* LISTINSERT_H_ */
