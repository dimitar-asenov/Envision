/***********************************************************************************************************************
 * NodeReadWriteLock.h
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NODEREADWRITELOCK_H_
#define NODEREADWRITELOCK_H_

#include "modelbase_api.h"
#include "InterruptibleThread.h"

#include <QReadWriteLock>
#include <QMutex>
#include <QList>

namespace Model {

class Node;

class MODELBASE_API NodeReadWriteLock
{
	private:
		QReadWriteLock lock;

		QMutex readersAccess;
		QList<InterruptibleThread*> interruptibleReaders;

	public:
		NodeReadWriteLock();
		virtual ~NodeReadWriteLock();

		void lockForRead();
		void lockForWrite(Node *targetNode);
		void unlock();
};

}

#endif /* NODEREADWRITELOCK_H_ */
