/***********************************************************************************************************************
 * SwitchCase.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/SwitchCase.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(SwitchCase, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(SwitchCase, Model::ExtendableNode)

REGISTER_ATTRIBUTE(SwitchCase, expr, Expression, false, false, true)
REGISTER_ATTRIBUTE(SwitchCase, statement, TypedListOfStatementItem, false, false, true)

}
