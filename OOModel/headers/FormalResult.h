/***********************************************************************************************************************
 * FormalResult.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FORMALRESULT_H_
#define FORMALRESULT_H_

#include "oomodel_api.h"

#include "common/attributeMacros.h"
#include "types/Type.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API FormalResult : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(FormalResult)
	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Type, type, setType)
};

}

#endif /* FORMALRESULT_H_ */
