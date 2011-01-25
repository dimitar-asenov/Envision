/***********************************************************************************************************************
 * PersistentStore.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PERSISTENTSTORE_H_
#define PERSISTENTSTORE_H_

#include "../modelbase_api.h"

#include <QtCore/QString>
#include <QtCore/QList>

namespace Model {

typedef long long NodeIdType;

class Node;
class Model;
class PersistedNode;

struct MODELBASE_API LoadedNode
{
	QString name;
	Node* node;
};

class MODELBASE_API PersistentStore
{
	friend class Model;

	protected:
		virtual void saveModel(Model* model, const QString &name) = 0;
		virtual Node* loadModel(Model* model, const QString &name) = 0;

	public:
		virtual void saveStringValue(const QString &value) = 0;
		virtual void saveIntValue(int value) = 0;
		virtual void saveDoubleValue(double value) = 0;
		virtual void saveNode(const Node *node, const QString &name, bool partialLoadHint) = 0;

		virtual QList<LoadedNode> loadAllSubNodes(Node* parent) = 0;
		virtual Node* loadSubNode(Node* parent, const QString& name) = 0;
		virtual QString currentNodeType() const = 0;
		virtual QList<LoadedNode> loadPartialNode(Node* partialNode) = 0;
		virtual PersistedNode* loadCompleteNodeSubtree(const QString& modelName, NodeIdType persistenceUnitId, NodeIdType nodeId) = 0;

		virtual int loadIntValue() = 0;
		virtual QString loadStringValue() = 0;
		virtual double loadDoubleValue() = 0;

		virtual ~PersistentStore() {};
};

}

#endif /* PERSISTENTSTORE_H_ */
