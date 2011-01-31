/***********************************************************************************************************************
 * BooleanLiteral.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOOLEANLITERAL_H_
#define BOOLEANLITERAL_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Boolean.h"

namespace OOModel {

class OOMODEL_API BooleanLiteral: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(BooleanLiteral)

	ATTRIBUTE_VALUE(::Model::Boolean, value, setValue, bool)
};


}

#endif /* BOOLEANLITERAL_H_ */
