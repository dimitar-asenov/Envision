/***********************************************************************************************************************
 * ExtendedNodeOptional.h
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDEDNODEOPTIONAL_H_
#define EXTENDEDNODEOPTIONAL_H_

#include "UndoCommand.h"
#include <QVector>

namespace Model {

class Node;

class ExtendedNodeOptional: public UndoCommand
{
	private:
		Node* attribute;
		int attributeIndex;
		QVector<Node*>* nodeAttributes;
		bool created;

	public:
		ExtendedNodeOptional(Node* target, Node* attribute, int attributeIndex, QVector<Node*>* nodeAttributes, bool created);
		virtual ~ExtendedNodeOptional();

		void redo();
		void undo();
};

}

#endif /* EXTENDEDNODEOPTIONAL_H_ */
