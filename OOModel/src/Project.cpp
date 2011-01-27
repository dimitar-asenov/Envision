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

Model::ExtendableIndex Project::projectsIndex = Model::ExtendableIndex();
Model::ExtendableIndex Project::modulesIndex = Model::ExtendableIndex();

void Project::init()
{
	registerNodeType();
	projectsIndex = registerNewAttribute("projects", "List", false, false, true);
	modulesIndex = registerNewAttribute("modules", "List", false, false, true);
}

}
