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

		void beginModification(const QString &text);
		void endModification();

		Node* getRoot();

		Node* createRoot(const QString &typeName);

		void pushCommandOnUndoStack(UndoCommand* command);
		void undo();
		void redo();

		/**
		 * Returns the model object that has as its root node the node indicated.
		 */
		static Model* getModel(Node* root);
};

}

#endif /* MODEL_H_ */
