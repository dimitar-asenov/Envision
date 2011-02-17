/***********************************************************************************************************************
 * Project.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PROJECT_H_
#define PROJECT_H_

#include "../oomodel_api.h"

#include "attributeMacros.h"
#include "Module.h"
#include "Class.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class Library;

class OOMODEL_API Project : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Project)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Model::TypedList<Project>, projects, setProjects)
	ATTRIBUTE(Model::TypedList<Library>, libraries, setLibraries)
	ATTRIBUTE(Model::TypedList<Module>, modules, setModules)
	ATTRIBUTE(Model::TypedList<Class>, classes, setClasses)

	public:
		virtual QString referenceName() const;
};

}

#endif /* PROJECT_H_ */
