/***********************************************************************************************************************
 * expressions/MethodCallExpression.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/MethodCallExpression.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(MethodCallExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(MethodCallExpression, Expression)

REGISTER_ATTRIBUTE(MethodCallExpression, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(MethodCallExpression, ref, OOReference, false, false, true)
REGISTER_ATTRIBUTE(MethodCallExpression, arguments, TypedListOfExpression, false, false, true)

}
