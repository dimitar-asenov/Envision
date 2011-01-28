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

Model::ExtendableIndex Module::nameIndex = Model::ExtendableIndex();
Model::ExtendableIndex Module::classesIndex = Model::ExtendableIndex();

void Module::init()
{
	registerNodeType();
	nameIndex = registerNewAttribute("name", "Text", false, false, true);
	classesIndex = registerNewAttribute("classes", "List", false, false, true);
}

}
