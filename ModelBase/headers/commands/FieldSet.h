/***********************************************************************************************************************
 * FieldSet.h
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FIELDSET_H_
#define FIELDSET_H_

#include "UndoCommand.h"
#include "../Model.h"

namespace Model {

template<class T>
class FieldSet: public UndoCommand
{
	protected:
		T& field;
		T oldValue;
		T newValue;

	public:
		FieldSet(Node *target, T& fieldToSet, T setTo) :
			UndoCommand(target, "Set field"), field(fieldToSet), oldValue(fieldToSet), newValue(setTo)
		{
		}

		void redo()
		{
			field = newValue;
			UndoCommand::redo();
		}

		void undo()
		{
			field = oldValue;
			UndoCommand::undo();
		}
};

}

#endif /* FIELDSET_H_ */
