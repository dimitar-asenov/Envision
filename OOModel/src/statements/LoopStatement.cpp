/***********************************************************************************************************************
 * LoopStatement.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/LoopStatement.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(LoopStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(LoopStatement, Statement)

REGISTER_ATTRIBUTE(LoopStatement, condition, Expression, false, true, true)
REGISTER_ATTRIBUTE(LoopStatement, initStep, Statement, false, true, true)
REGISTER_ATTRIBUTE(LoopStatement, updateStep, Statement, false, true, true)
REGISTER_ATTRIBUTE(LoopStatement, body, Statement, false, false, true)

}
