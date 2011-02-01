/***********************************************************************************************************************
 * ExtendedNodeChild.h
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDEDNODECHILD_H_
#define EXTENDEDNODECHILD_H_

#include "UndoCommand.h"
#include "../nodes/Extendable/ExtendableIndex.h"
#include <QtCore/QVector>

namespace Model {

class Node;

class ExtendedNodeChild: public UndoCommand
{
	private:
		Node* newVal;
		Node* oldVal;
		ExtendableIndex attributeIndex;
		QVector< QVector<Node*> >* subnodes;

	public:
		ExtendedNodeChild(Node* target, Node* newValue, const ExtendableIndex &attributeIndex, QVector< QVector<Node*> >* subnodes);
		virtual ~ExtendedNodeChild();

		virtual void redo();
		virtual void undo();
};

}

#endif /* EXTENDEDNODECHILD_H_ */
