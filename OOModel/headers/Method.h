/***********************************************************************************************************************
 * Method.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHOD_H_
#define METHOD_H_

#include "oomodel_api.h"

#include "common/attributeMacros.h"
#include "common/Visibility.h"
#include "MethodItem.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Method : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Method)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Model::TypedList<MethodItem>, items);
	ATTRIBUTE_OOP_VISIBILITY

	public:
		virtual QString referenceName() const;
};

}

#endif /* METHOD_H_ */
