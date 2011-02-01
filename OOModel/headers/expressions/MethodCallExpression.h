/***********************************************************************************************************************
 * MethodCallExpression.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODCALLEXPRESSION_H_
#define METHODCALLEXPRESSION_H_

#include "Expression.h"
#include "../OOReference.h"
#include "../types/Type.h"
#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API MethodCallExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(MethodCallExpression)

	ATTRIBUTE(Expression, prefix, setPrefix)
	ATTRIBUTE(OOReference, ref, setRef)
	ATTRIBUTE(Model::TypedList<Expression>, arguments, setArguments)
};

}

#endif /* METHODCALLEXPRESSION_H_ */
