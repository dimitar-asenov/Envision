/***********************************************************************************************************************
 * ArrayInitializer.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/ArrayInitializer.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ArrayInitializer, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ArrayInitializer, Expression)

REGISTER_ATTRIBUTE(ArrayInitializer, values, TypedListOfExpression, false, false, true)

}
