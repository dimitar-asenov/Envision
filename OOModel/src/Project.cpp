/***********************************************************************************************************************
 * Project.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Project.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Project, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Project, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Project, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Project, projects, List, false, false, true)
REGISTER_ATTRIBUTE(Project, modules, List, false, false, true)

QString Project::referenceName() const
{
	return name();
}
}
