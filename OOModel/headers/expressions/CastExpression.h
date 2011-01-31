/***********************************************************************************************************************
 * CastExpression.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CASTEXPRESSION_H_
#define CASTEXPRESSION_H_

#include "Expression.h"
#include "../types/Type.h"

namespace OOModel {

class OOMODEL_API CastExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(CastExpression)

	ATTRIBUTE(Type, type);
	ATTRIBUTE(Expression, expr);
};

}

#endif /* CASTEXPRESSION_H_ */
