/***********************************************************************************************************************
 * StringLiteral.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/StringLiteral.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(StringLiteral, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(StringLiteral, Expression)

REGISTER_ATTRIBUTE(StringLiteral, value, Text, false, false, true)

}
