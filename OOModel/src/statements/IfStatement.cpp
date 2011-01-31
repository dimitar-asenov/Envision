/***********************************************************************************************************************
 * IfStatement.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/IfStatement.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(IfStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(IfStatement, Statement)

REGISTER_ATTRIBUTE(IfStatement, condition, Expression, false, false, true)
REGISTER_ATTRIBUTE(IfStatement, thenBranch, Statement, false, false, true)
REGISTER_ATTRIBUTE(IfStatement, elseBranch, Statement, false, true, true)

}
