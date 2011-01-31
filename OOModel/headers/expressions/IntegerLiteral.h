/***********************************************************************************************************************
 * IntegerLiteral.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INTEGERLITERAL_H_
#define INTEGERLITERAL_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Integer.h"

namespace OOModel {

class OOMODEL_API IntegerLiteral: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(IntegerLiteral)

	ATTRIBUTE_VALUE(::Model::Integer, value, setValue, int)
};

}

#endif /* INTEGERLITERAL_H_ */
