/***********************************************************************************************************************
 * FloatLiteral.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FLOATLITERAL_H_
#define FLOATLITERAL_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Float.h"

namespace OOModel {

class OOMODEL_API FloatLiteral: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(FloatLiteral)

	ATTRIBUTE_VALUE(::Model::Float, value, setValue, double)
};

}

#endif /* FLOATLITERAL_H_ */
