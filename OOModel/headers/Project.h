/***********************************************************************************************************************
 * Project.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PROJECT_H_
#define PROJECT_H_

#include "oomodel_api.h"

#include "common/attributeMacros.h"
#include "Module.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Project : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Project)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Model::TypedList<Project>, projects)
	ATTRIBUTE(Model::TypedList<Module>, modules)

	public:
		virtual QString referenceName() const;
};

}

#endif /* PROJECT_H_ */
