/***********************************************************************************************************************
 * MethodArgument.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "MethodArgument.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(MethodArgument, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(MethodArgument, Model::ExtendableNode)

REGISTER_ATTRIBUTE(MethodArgument, type, Type, false, false, true)
REGISTER_ATTRIBUTE(MethodArgument, name, Text, false, false, true)

}
