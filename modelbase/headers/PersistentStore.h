/***********************************************************************************************************************
 * PersistentStore.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PERSISTENTSTORE_H_
#define PERSISTENTSTORE_H_

#include "modelbase_api.h"
#include <QString>

namespace Model {

class Node;

struct LoadedNode
{
	QString name;
	Node* node;
};

class MODELBASE_API PersistentStore
{
	public:
		virtual ~PersistentStore() = 0;

		virtual void saveStringValue(QString &value) = 0;
		virtual void saveIntValue(int &value) = 0;
		virtual void saveFloatValue(double &value) = 0;
		virtual void saveSubNode(Node *subnode, QString &name) = 0;

		virtual bool hasMoreSubNodes() = 0;
		virtual LoadedNode getNextSubNode() = 0;

		virtual Node* getRootNode() = 0;
};

}

#endif /* PERSISTENTSTORE_H_ */
