/***********************************************************************************************************************
 * ForEachStatement.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/ForEachStatement.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ForEachStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ForEachStatement, Statement)

REGISTER_ATTRIBUTE(ForEachStatement, varName, Text, false, false, true)
REGISTER_ATTRIBUTE(ForEachStatement, varType, Type, false, true, true)
REGISTER_ATTRIBUTE(ForEachStatement, collection, Expression, false, false, true)
REGISTER_ATTRIBUTE(ForEachStatement, body, TypedListOfStatementItem, false, false, true)

}
