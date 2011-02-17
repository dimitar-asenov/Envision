/***********************************************************************************************************************
 * ArrayType.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "types/ArrayType.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ArrayType, Type)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ArrayType, Type)

REGISTER_ATTRIBUTE(ArrayType, type, Type, false, false, true)

}
