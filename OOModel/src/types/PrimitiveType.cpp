/***********************************************************************************************************************
 * PrimitiveType.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "types/PrimitiveType.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(PrimitiveType, Type)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(PrimitiveType, Type)

REGISTER_ATTRIBUTE(PrimitiveType, val, Integer, false, false, true)

}
