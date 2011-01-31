/***********************************************************************************************************************
 * SwitchStatement.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/SwitchStatement.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(SwitchStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(SwitchStatement, Statement)

REGISTER_ATTRIBUTE(SwitchStatement, switchVar, Expression, false, false, true)
REGISTER_ATTRIBUTE(SwitchStatement, cases, TypedListOfSwitchCase, false, false, true)

}
