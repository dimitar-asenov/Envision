/***********************************************************************************************************************
 * FloatLiteral.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/FloatLiteral.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(FloatLiteral, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(FloatLiteral, Expression)

REGISTER_ATTRIBUTE(FloatLiteral, value, Float, false, false, true)


}
