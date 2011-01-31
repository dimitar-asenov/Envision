/***********************************************************************************************************************
 * NewExpression.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NEWEXPRESSION_H_
#define NEWEXPRESSION_H_

#include "Expression.h"
#include "../OOReference.h"
#include "../types/Type.h"

namespace OOModel {

class OOMODEL_API NewExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(NewExpression)

	ATTRIBUTE(Type, type);
	ATTRIBUTE(Expression, amount);
};

}

#endif /* NEWEXPRESSION_H_ */
