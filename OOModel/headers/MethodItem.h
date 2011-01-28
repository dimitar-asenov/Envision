/***********************************************************************************************************************
 * MethodItem.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODITEM_H_
#define METHODITEM_H_

#include "oomodel_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API MethodItem : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(MethodItem)
};

}

#endif /* METHODITEM_H_ */
