/***********************************************************************************************************************
 * Persistent.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PERSISTENT_H_
#define PERSISTENT_H_

#include "modelbase_api.h"
#include "PersistentStore.h"
#include <QString>

namespace Model {

class MODELBASE_API Persistent
{
	public:
		virtual ~Persistent() = 0;

		/**
		 * Saves the current node to a persistent store.
		 *
		 *
		 * @param store
		 * 				The persistent store to use
		 * @param name
		 * 				The name under which to save this node. This can be QString::null to indicate that the node has no
		 * 				name. If the node has no name it is assumed that is a part of an ordered list.
		 *
		 */
		virtual void save(PersistentStore &store, QString &name) = 0;
		virtual void load(PersistentStore &store) = 0;
};

}

#endif /* PERSISTENT_H_ */
