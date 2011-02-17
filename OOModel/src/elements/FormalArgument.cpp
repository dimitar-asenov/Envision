/***********************************************************************************************************************
 * FormalArgument.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/FormalArgument.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(FormalArgument, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(FormalArgument, Model::ExtendableNode)

REGISTER_ATTRIBUTE(FormalArgument, name, Text, false, false, true)
REGISTER_ATTRIBUTE(FormalArgument, type, Type, false, false, true)


}
