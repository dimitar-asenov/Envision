/***********************************************************************************************************************
 * UnaryOperation.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/UnaryOperation.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(UnaryOperation, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(UnaryOperation, Expression)

REGISTER_ATTRIBUTE(UnaryOperation, operand, Expression, false, false, true)
REGISTER_ATTRIBUTE(UnaryOperation, opr, Integer, false, false, true)

}
