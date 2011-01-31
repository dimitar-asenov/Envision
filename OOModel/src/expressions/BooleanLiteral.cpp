/***********************************************************************************************************************
 * BooleanLiteral.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/BooleanLiteral.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BooleanLiteral, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BooleanLiteral, Expression)

REGISTER_ATTRIBUTE(BooleanLiteral, value, Boolean, false, false, true)

}
