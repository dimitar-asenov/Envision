/***********************************************************************************************************************
 * NodeReadWriteLock.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "NodeReadWriteLock.h"
#include "ModelException.h"
#include <QMutexLocker>

namespace Model {

NodeReadWriteLock::NodeReadWriteLock() :
	lock(QReadWriteLock::Recursive)
{
}

NodeReadWriteLock::~NodeReadWriteLock()
{
}

void NodeReadWriteLock::lockForRead()
{
	InterruptibleThread* interruptibleThread = dynamic_cast<InterruptibleThread*> (QThread::currentThread());

	if ( interruptibleThread )
	{
		QMutexLocker locker(&readersAccess);
		interruptibleReaders.append(interruptibleThread);
	}

	lock.lockForRead();
}

void NodeReadWriteLock::lockForWrite(Node* targetNode)
{
	{
		QMutexLocker locker(&readersAccess);
		for (QList<InterruptibleThread*>::iterator r = interruptibleReaders.begin(); r != interruptibleReaders.end(); ++r)
			(*r)->requestInterrupt(targetNode);
	}
	lock.lockForWrite();
}

void NodeReadWriteLock::unlock()
{
	InterruptibleThread* interruptibleThread = dynamic_cast<InterruptibleThread*> (QThread::currentThread());
	if ( interruptibleThread )
	{
		QMutexLocker locker(&readersAccess);
		interruptibleReaders.removeOne(interruptibleThread);
	}

	lock.unlock();
}

}
