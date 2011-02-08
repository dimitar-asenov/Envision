/***********************************************************************************************************************
 * NamedType.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "types/NamedType.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(NamedType, Type)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(NamedType, Type)

REGISTER_ATTRIBUTE(NamedType, type, Reference, false, false, true)

}
