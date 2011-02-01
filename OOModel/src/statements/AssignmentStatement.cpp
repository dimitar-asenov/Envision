/***********************************************************************************************************************
 * AssignmentStatement.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/AssignmentStatement.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(AssignmentStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(AssignmentStatement, Statement)

REGISTER_ATTRIBUTE(AssignmentStatement, left, Expression, false, false, true)
REGISTER_ATTRIBUTE(AssignmentStatement, right, Expression, false, false, true)
REGISTER_ATTRIBUTE(AssignmentStatement, opr, Integer, false, false, true)

}
