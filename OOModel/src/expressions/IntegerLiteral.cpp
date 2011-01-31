/***********************************************************************************************************************
 * IntegerLiteral.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/IntegerLiteral.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(IntegerLiteral, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(IntegerLiteral, Expression)

REGISTER_ATTRIBUTE(IntegerLiteral, value, Integer, false, false, true)

}
