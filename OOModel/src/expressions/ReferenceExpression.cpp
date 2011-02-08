/***********************************************************************************************************************
 * ReferenceExpression.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/ReferenceExpression.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ReferenceExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ReferenceExpression, Expression)

REGISTER_ATTRIBUTE(ReferenceExpression, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(ReferenceExpression, ref, Reference, false, false, true)

}
