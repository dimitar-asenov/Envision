/***********************************************************************************************************************
 * MethodCallExpression.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODCALLEXPRESSION_H_
#define METHODCALLEXPRESSION_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Reference.h"
#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class Method;

class OOMODEL_API MethodCallExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(MethodCallExpression)

	ATTRIBUTE(Expression, prefix, setPrefix)
	ATTRIBUTE(Model::Reference, ref, setRef)
	ATTRIBUTE(Model::TypedList<Expression>, arguments, setArguments)

	public:
		Method* methodDefinition();
};

}

#endif /* METHODCALLEXPRESSION_H_ */
