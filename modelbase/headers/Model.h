/***********************************************************************************************************************
 * Model.h
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODEL_H_
#define MODEL_H_

#include "modelbase_api.h"
#include "Node.h"

#include <QUndoStack>
#include <QMutex>
#include <QVector>

namespace Model {

class UndoCommand;

class MODELBASE_API Model
{
	private:
		Node* root;
		QUndoStack commands;
		QMutex modification;

		static QList<Model*> loadedModels;

	public:
		Model();
		virtual ~Model();

		void beginModification();
		void endModification();

		Node* getRoot();

		void pushCommandOnUndoStack(UndoCommand* command);

		/**
		 * Returns the model object that has as its root node the node indicated.
		 */
		static Model* getModel(Node* root);
};

}

#endif /* MODEL_H_ */
