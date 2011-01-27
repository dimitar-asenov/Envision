/***********************************************************************************************************************
 * Module.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Module.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Module, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Module, Model::ExtendableNode)

Model::ExtendableIndex Module::classesIndex = Model::ExtendableIndex();

void Module::init()
{
	registerNodeType();
	classesIndex = registerNewAttribute("classes", "List", false, false, true);
}

}
