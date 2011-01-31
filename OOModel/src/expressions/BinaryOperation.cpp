/***********************************************************************************************************************
 * BinaryOperation.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/BinaryOperation.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryOperation, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryOperation, Expression)

REGISTER_ATTRIBUTE(BinaryOperation, left, Expression, false, false, true)
REGISTER_ATTRIBUTE(BinaryOperation, right, Expression, false, false, true)
REGISTER_ATTRIBUTE(BinaryOperation, opr, Integer, false, false, true)

}
