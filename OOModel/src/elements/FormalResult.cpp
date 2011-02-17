/***********************************************************************************************************************
 * FormalResult.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/FormalResult.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(FormalResult, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(FormalResult, Model::ExtendableNode)

REGISTER_ATTRIBUTE(FormalResult, name, Text, false, false, true)
REGISTER_ATTRIBUTE(FormalResult, type, Type, false, false, true)

}
