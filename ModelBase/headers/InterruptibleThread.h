/***********************************************************************************************************************
 * InterruptibleThread.h
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INTERRUPTIBLETHREAD_H_
#define INTERRUPTIBLETHREAD_H_

#include "modelbase_api.h"
#include <QtCore/QThread>

namespace Model {

class Node;
class MODELBASE_API InterruptibleThread: public QThread
{
	public:

		/**
		 * Requests an interrupt in the operation of the thread due to a writer needing access to the specified node.
		 *
		 * A interruptible thread uses this method to implement interrupt behavior. This could include setting an
		 * interrupt flag, or posting an interrupt message on the thread's message queue if event processing was started
		 * with exec().
		 *
		 * Threads which perform long operations (such as verification) should generally be interruptible to allow the
		 * user to interact with the system.
		 *
		 * NOTE: If the thread has acquired the lock corresponding to writeRequestTarget multiple times, this method
		 * will be called once for each nested acquire operation.
		 */
		virtual void requestInterrupt(Node* writeRequestTarget) = 0;
};

}

#endif /* INTERRUPTIBLETHREAD_H_ */
