/***********************************************************************************************************************
 * Field.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FIELD_H_
#define FIELD_H_

#include "../oomodel_api.h"

#include "../attributeMacros.h"
#include "../elements/Visibility.h"
#include "../elements/Static.h"
#include "../types/Type.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Field : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Field)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Type, type, setType)
	ATTRIBUTE_OOP_VISIBILITY
	ATTRIBUTE_OOP_STATIC

	public:
		virtual bool definesSymbol() const;
		virtual const QString& symbolName() const;
};


}

#endif /* FIELD_H_ */
