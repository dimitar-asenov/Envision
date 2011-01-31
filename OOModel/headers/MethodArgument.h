/***********************************************************************************************************************
 * MethodArgument.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODARGUMENT_H_
#define METHODARGUMENT_H_

#include "oomodel_api.h"

#include "common/attributeMacros.h"
#include "types/Type.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API MethodArgument : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(MethodArgument)
	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Type, type)
};

}

#endif /* METHODARGUMENT_H_ */
