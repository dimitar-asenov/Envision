/***********************************************************************************************************************
 * expressions/MethodCall.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/MethodCall.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(MethodCall, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(MethodCall, Expression)

REGISTER_ATTRIBUTE(MethodCall, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(MethodCall, ref, OOReference, false, false, true)
REGISTER_ATTRIBUTE(MethodCall, arguments, TypedListOfExpression, false, false, true)

}
