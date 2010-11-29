/***********************************************************************************************************************
 * Model.h
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODEL_H_
#define MODEL_H_

#include "modelbase_api.h"
#include "nodes/Node.h"
#include "NodeReadWriteLock.h"

#include <QString>
#include <QUndoStack>
#include <QMutex>
#include <QVector>

namespace Model {

class UndoCommand;

class MODELBASE_API Model: public QObject
{
	Q_OBJECT

	private:
		Node* root;
		QUndoStack commands;

		QMutex exclusiveAccess;
		NodeReadWriteLock rootLock;

		QString modificationText;
		Node* currentModificationTarget;
		NodeReadWriteLock* currentModificationLock;
		QList<Node*> modifiedTargets;

		bool pushedNewCommandsOnTheStack;
		bool modificationInProgress;

		NodeIdType nextId;

		static QList<Model*> loadedModels;

	public:
		Model();
		virtual ~Model();

		void beginModification(Node* modificationTarget, const QString &text = QString());
		void endModification();
		void changeModificationTarget(Node* modificationTarget);

		void beginExclusiveRead();
		void endExclusiveRead();

		NodeReadWriteLock* getRootLock();

		bool isInCurrentAccessUnit(const Node* node) const;

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

		void emitNameModified(Node* node, const QString &oldName);

	signals:
		void rootCreated(Node* root);
		void nodesModified(QList<Node*> nodes);
		void nameModified(Node* node, const QString &oldName);

};

}

#endif /* MODEL_H_ */
