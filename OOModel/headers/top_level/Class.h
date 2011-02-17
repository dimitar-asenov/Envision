/***********************************************************************************************************************
 * Class.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CLASS_H_
#define CLASS_H_

#include "../oomodel_api.h"

#include "attributeMacros.h"
#include "elements/Visibility.h"
#include "Method.h"
#include "Field.h"
#include "types/Type.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Class : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Class)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Model::TypedList<Type>, baseClasses, setBaseClasses)
	ATTRIBUTE(Model::TypedList<Field>, fields, setFields)
	ATTRIBUTE(Model::TypedList<Method>, methods, setMethods)
	ATTRIBUTE_OOP_VISIBILITY

	public:
		virtual QString referenceName() const;
};

}

#endif /* CLASS_H_ */
