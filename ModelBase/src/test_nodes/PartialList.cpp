/***********************************************************************************************************************
 * PartialList.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/PartialList.h"

namespace TestNodes {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(PartialList, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(PartialList, Model::ExtendableNode)

REGISTER_ATTRIBUTE(PartialList, list, List, true, false, true)

}
