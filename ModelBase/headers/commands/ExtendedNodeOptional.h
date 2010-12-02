/***********************************************************************************************************************
 * ExtendedNodeOptional.h
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDEDNODEOPTIONAL_H_
#define EXTENDEDNODEOPTIONAL_H_

#include "nodes/Extendable/ExtendableIndex.h"
#include "commands/UndoCommand.h"
#include <QtCore/QVector>

namespace Model {

class Node;

class ExtendedNodeOptional: public UndoCommand
{
	private:
		Node* attribute;
		ExtendableIndex attributeIndex;
		QVector< QVector<Node*> >* subnodes;
		bool created;

	public:
		ExtendedNodeOptional(Node* target, Node* attribute, const ExtendableIndex &attributeIndex, QVector< QVector<Node*> >* subnodes, bool created);
		virtual ~ExtendedNodeOptional();

		void redo();
		void undo();
};

}

#endif /* EXTENDEDNODEOPTIONAL_H_ */
