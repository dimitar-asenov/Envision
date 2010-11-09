/***********************************************************************************************************************
 * TextSet.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXTSET_H_
#define TEXTSET_H_

#include "UndoCommand.h"
#include <QString>

namespace Model {

class TextSet: public Model::UndoCommand
{
	private:
		QString* field;
		QString oldText;
		QString newText;

	public:
		TextSet(Node *target, QString *field, QString setTo);

		void redo();
		void undo();
};

}

#endif /* TEXTSET_H_ */
