/***********************************************************************************************************************
 * Module.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODULE_H_
#define MODULE_H_

#include "oomodel_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Module : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Module)

	private:
		static Model::ExtendableIndex classesIndex;

	public:
		static void init();

		Model::List* classes();
};

inline Model::List* Module::classes() { return static_cast<Model::List*> (get(classesIndex)); }

}

#endif /* MODULE_H_ */
