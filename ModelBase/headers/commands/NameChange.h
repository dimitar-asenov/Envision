/***********************************************************************************************************************
 * NameChange.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NAMECHANGE_H_
#define NAMECHANGE_H_

#include <QtCore/QString>

#include "UndoCommand.h"

namespace Model {

class NameChange: public UndoCommand
{
	protected:
		QString oldName;
		QString newName;
		UndoCommand* command;

	public:
		NameChange(Node *target, const QString& oldName, const QString& newName, UndoCommand* command);
		~NameChange();
		void redo();
		void undo();
};

}

#endif /* NAMECHANGE_H_ */
