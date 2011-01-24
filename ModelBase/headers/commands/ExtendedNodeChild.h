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
		Node* attribute;
		ExtendableIndex attributeIndex;
		QVector< QVector<Node*> >* subnodes;
		bool created;

	public:
		ExtendedNodeChild(Node* target, Node* attribute, const ExtendableIndex &attributeIndex, QVector< QVector<Node*> >* subnodes, bool created);
		virtual ~ExtendedNodeChild();

		void redo();
		void undo();
};

}

#endif /* EXTENDEDNODECHILD_H_ */
