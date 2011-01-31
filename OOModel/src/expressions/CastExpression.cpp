/***********************************************************************************************************************
 * CastExpression.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/CastExpression.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(CastExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(CastExpression, Expression)

REGISTER_ATTRIBUTE(CastExpression, type, Type, false, false, true)
REGISTER_ATTRIBUTE(CastExpression, expr, Expression, false, true, true)

}
