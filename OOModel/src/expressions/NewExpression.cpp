/***********************************************************************************************************************
 * NewExpression.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/NewExpression.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(NewExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(NewExpression, Expression)

REGISTER_ATTRIBUTE(NewExpression, type, Type, false, false, true)
REGISTER_ATTRIBUTE(NewExpression, amount, Expression, false, true, true)
}
