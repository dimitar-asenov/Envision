/***********************************************************************************************************************
 * Class.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CLASS_H_
#define CLASS_H_

#include "oomodel_api.h"

#include "common/attributeMacros.h"
#include "common/Visibility.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Class : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Class)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Model::List, fields)
	ATTRIBUTE(Model::List, methods)
	ATTRIBUTE_OOP_VISIBILITY

	public:
		virtual QString referenceName() const;
};

}

#endif /* CLASS_H_ */
