/***********************************************************************************************************************
 * IntegerSet.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INTEGERSET_H_
#define INTEGERSET_H_

#include "UndoCommand.h"

namespace Model {

class IntegerSet: public UndoCommand
{
	private:
		int* field;
		int oldValue;
		int newValue;

	public:
		IntegerSet(Node *target, int *field, int setTo);

		void redo();
		void undo();
};

}

#endif /* INTEGERSET_H_ */
