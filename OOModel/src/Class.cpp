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

Model::ExtendableIndex Class::nameIndex = Model::ExtendableIndex();
Model::ExtendableIndex Class::fieldsIndex = Model::ExtendableIndex();
Model::ExtendableIndex Class::methodsIndex = Model::ExtendableIndex();
Model::ExtendableIndex Class::visibilityIndex = Model::ExtendableIndex();

void Class::init()
{
	registerNodeType();
	nameIndex = registerNewAttribute("name", "Text", false, false, true);
	fieldsIndex = registerNewAttribute("fields", "List", false, false, true);
	methodsIndex = registerNewAttribute("methods", "List", false, false, true);
	visibilityIndex = registerNewAttribute("visibility", "Visibility", false, false, true);
}

}
