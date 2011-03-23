/***********************************************************************************************************************
 * VariableDeclaration.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VariableDeclaration.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(VariableDeclaration, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(VariableDeclaration, Statement)

REGISTER_ATTRIBUTE(VariableDeclaration, name, Text, false, false, true)
REGISTER_ATTRIBUTE(VariableDeclaration, type, Type, false, false, true)
REGISTER_ATTRIBUTE(VariableDeclaration, initialValue, Expression, false, true, true)

bool VariableDeclaration::definesSymbol() const
{
	return true;
}

const QString& VariableDeclaration::symbolName() const
{
	return name();
}

}
