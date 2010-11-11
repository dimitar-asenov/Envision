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

#include <QString>
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

		QString modificationText;
		bool pushedNewCommandsOnTheStack;
		bool modificationInProgress;
		QMutex modification;

		NodeIdType nextId;

		static QList<Model*> loadedModels;

	public:
		Model();
		virtual ~Model();

		void beginModification(const QString &text = QString());
		void endModification();

		Node* getRoot();

		NodeIdType generateNextId();

		Node* createRoot(const QString &typeName);

		void pushCommandOnUndoStack(UndoCommand* command);
		void undo();
		void redo();

		void save(PersistentStore& store);

		/**
		 * Returns the model object that has as its root node the node indicated.
		 */
		static Model* getModel(Node* root);
};

}

#endif /* MODEL_H_ */
