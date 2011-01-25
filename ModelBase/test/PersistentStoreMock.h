/***********************************************************************************************************************
 * PersistentStoreMock.h
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PERSISTENTSTOREMOCK_H_
#define PERSISTENTSTOREMOCK_H_

#include "persistence/PersistentStore.h"

namespace Model {

class PersistentStoreMock: public PersistentStore
{
	private:
		QString savedData;

	public:
		PersistentStoreMock();
		~PersistentStoreMock();


		virtual void saveStringValue(const QString &value);
		virtual void saveIntValue(int value);
		virtual void saveDoubleValue(double value);
		virtual void saveNode(const Node *node, const QString &name, bool partialLoadHint);


		virtual QList<LoadedNode> loadAllSubNodes(Node* parent);
		virtual Node* loadSubNode(Node* parent, const QString& name);
		virtual QList<LoadedNode> loadPartialNode(Node* partialNode);
		virtual PersistedNode* loadCompleteNodeSubtree(const QString& modelName, NodeIdType persistenceUnitId, NodeIdType nodeId);
		virtual QString currentNodeType() const;

		virtual int loadIntValue();
		virtual QString loadStringValue();
		virtual double loadDoubleValue();

		const QString& getSaved() const;
		void clear();

	protected:
		virtual void saveModel(Model* model, const QString &name);
		virtual Node* loadModel(Model* model, const QString &name);

};

}

#endif /* PERSISTENTSTOREMOCK_H_ */
