/***********************************************************************************************************************
 * Module.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODULE_H_
#define MODULE_H_

#include "oomodel_api.h"

#include "common/attributeMacros.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Module : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Module)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Model::List, classes);

	public:
		static void init();
};

}

#endif /* MODULE_H_ */
