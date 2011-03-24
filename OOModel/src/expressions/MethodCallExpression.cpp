/***********************************************************************************************************************
 * expressions/MethodCallExpression.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/MethodCallExpression.h"
#include "top_level/Class.h"
#include "top_level/Method.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(MethodCallExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(MethodCallExpression, Expression)

REGISTER_ATTRIBUTE(MethodCallExpression, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(MethodCallExpression, ref, Reference, false, false, true)
REGISTER_ATTRIBUTE(MethodCallExpression, arguments, TypedListOfExpression, false, false, true)

Method* MethodCallExpression::methodDefinition()
{
	Method* met = NULL;

	if (prefix())
	{
		Class* classNode = prefix()->classDefinition();

		if (classNode)	met = dynamic_cast<Method*> (classNode->navigateTo(classNode, ref()->path()));
	}
	else
		met = dynamic_cast<Method*> (ref()->get());

	return met;
}

}
