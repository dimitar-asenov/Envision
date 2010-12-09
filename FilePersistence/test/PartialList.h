/***********************************************************************************************************************
 * PartialList.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PARTIALLIST_H_
#define PARTIALLIST_H_

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/nodeMacros.h"
#include "ModelBase/headers/nodes/List.h"

namespace FilePersistence {

class PartialList: public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(PartialList)

	private:
		static Model::ExtendableIndex listIndex;

	public:
		static void init();

		Model::List* list();
};

}

#endif /* PARTIALLIST_H_ */
