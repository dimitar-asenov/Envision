/***********************************************************************************************************************
 * MethodCallStatement.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/MethodCallStatement.h"
#include "top_level/Class.h"
#include "top_level/Method.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(MethodCallStatement, Statement)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(MethodCallStatement, Statement)

REGISTER_ATTRIBUTE(MethodCallStatement, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(MethodCallStatement, ref, Reference, false, false, true)
REGISTER_ATTRIBUTE(MethodCallStatement, arguments, TypedListOfExpression, false, false, true)

Method* MethodCallStatement::methodDefinition()
{
	Method* met = NULL;

	if (prefix())
	{
		Class* classNode = prefix()->classDefinition();
		if (classNode)	met = dynamic_cast<Method*> (classNode->navigateTo(classNode, ref()->path()));
	}
	else
		met = dynamic_cast<Method*> (ref()->get());

	return met;
}

}
