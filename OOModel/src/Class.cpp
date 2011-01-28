/***********************************************************************************************************************
 * Class.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Class.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Class, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Class, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Class, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Class, fields, List, false, false, true)
REGISTER_ATTRIBUTE(Class, methods, List, false, false, true)
REGISTER_ATTRIBUTE(Class, visibility, Visibility, false, false, true)

}
