/***********************************************************************************************************************
 * CharacterLiteral.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/CharacterLiteral.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(CharacterLiteral, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(CharacterLiteral, Expression)

REGISTER_ATTRIBUTE(CharacterLiteral, value, Character, false, false, true)

}
