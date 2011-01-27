/***********************************************************************************************************************
 * Project.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PROJECT_H_
#define PROJECT_H_

#include "oomodel_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Project : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Project)

	private:
		static Model::ExtendableIndex projectsIndex;
		static Model::ExtendableIndex modulesIndex;

	public:
		static void init();

		Model::List* projects();
		Model::List* modules();
};

inline Model::List* Project::projects() { return static_cast<Model::List*> (get(projectsIndex)); }
inline Model::List* Project::modules() { return static_cast<Model::List*> (get(modulesIndex)); }

}

#endif /* PROJECT_H_ */
