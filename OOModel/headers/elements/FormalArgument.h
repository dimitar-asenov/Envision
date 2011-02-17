/***********************************************************************************************************************
 * FormalArgument.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FORMALARGUMENT_H_
#define FORMALARGUMENT_H_

#include "../oomodel_api.h"

#include "attributeMacros.h"
#include "types/Type.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API FormalArgument : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(FormalArgument)
	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Type, type, setType)
};

}

#endif /* FORMALARGUMENT_H_ */
