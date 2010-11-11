/***********************************************************************************************************************
 * PersistentStoreMock.h
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PERSISTENTSTOREMOCK_H_
#define PERSISTENTSTOREMOCK_H_

#include "PersistentStore.h"

namespace Model {

class PersistentStoreMock: public PersistentStore
{
	private:
		QString savedData;

	public:
		PersistentStoreMock();
		~PersistentStoreMock();

		void saveStringValue(const QString &value);
		void saveIntValue(int value);
		void saveFloatValue(double value);
		void saveNode(const Node *node, const QString &name, bool partialLoadHint);

		Node* loadRootNode(const QString &name);
		QList<LoadedNode> loadAllSubNodes(Node* parent);
		int loadIntValue();
		QString loadStringValue();
		double loadFloatValue();


		const QString& getSaved() const;
		void clear();
};

}

#endif /* PERSISTENTSTOREMOCK_H_ */
