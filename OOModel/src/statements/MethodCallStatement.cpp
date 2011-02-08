/***********************************************************************************************************************
 * MethodCallStatement.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/MethodCallStatement.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(MethodCallStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(MethodCallStatement, Statement)

REGISTER_ATTRIBUTE(MethodCallStatement, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(MethodCallStatement, ref, Reference, false, false, true)
REGISTER_ATTRIBUTE(MethodCallStatement, arguments, TypedListOfExpression, false, false, true)

}
