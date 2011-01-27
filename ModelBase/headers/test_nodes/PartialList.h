/***********************************************************************************************************************
 * PartialList.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PARTIALLIST_H_
#define PARTIALLIST_H_

#include "../modelbase_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/nodeMacros.h"
#include "ModelBase/headers/nodes/List.h"

namespace TestNodes {

class MODELBASE_API PartialList: public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(PartialList)

	private:
		static Model::ExtendableIndex listIndex;

	public:
		static void init();

		Model::List* list();
};

inline Model::List* PartialList::list() { return static_cast<Model::List*> (get(listIndex)); }

}

#endif /* PARTIALLIST_H_ */
