/***********************************************************************************************************************
 * ClipboardStore.h
 *
 *  Created on: Jan 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CLIPBOARDSTORE_H_
#define CLIPBOARDSTORE_H_

#include "../modelbase_api.h"

#include "PersistentStore.h"

namespace Model {

class MODELBASE_API ClipboardStore: public PersistentStore
{
	public:
		virtual void putNode(const Node* node) = 0;
		virtual void putNodes(const QList<const Node*>& nodes) = 0;
};

}

#endif /* CLIPBOARDSTORE_H_ */
