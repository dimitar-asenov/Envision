/***********************************************************************************************************************
 * VariableAccess.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VariableAccess.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(VariableAccess, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(VariableAccess, Expression)

REGISTER_ATTRIBUTE(VariableAccess, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(VariableAccess, ref, OOReference, false, false, true)


}
