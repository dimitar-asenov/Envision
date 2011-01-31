/***********************************************************************************************************************
 * ReturnStatement.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/ReturnStatement.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ReturnStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ReturnStatement, Statement)

REGISTER_ATTRIBUTE(ReturnStatement, value, Expression, false, false, true)

}
