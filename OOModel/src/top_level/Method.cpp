/***********************************************************************************************************************
 * Method.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/Method.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Method, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Method, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Method, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Method, items, StatementItemList, false, false, true)
REGISTER_ATTRIBUTE(Method, arguments, TypedListOfFormalArgument, false, false, true)
REGISTER_ATTRIBUTE(Method, results, TypedListOfFormalResult, false, false, true)
REGISTER_ATTRIBUTE(Method, visibility, Visibility, false, false, true)
REGISTER_ATTRIBUTE(Method, stat, Static, false, false, true)

bool Method::definesSymbol() const
{
	return true;
}

const QString& Method::symbolName() const
{
	return name();
}

}
