/***********************************************************************************************************************
 * Method.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Method.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Method, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Method, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Method, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Method, items, TypedListOfMethodItem, false, false, true)
REGISTER_ATTRIBUTE(Method, arguments, TypedListOfFormalArgument, false, false, true)
REGISTER_ATTRIBUTE(Method, results, TypedListOfFormalResult, false, false, true)
REGISTER_ATTRIBUTE(Method, visibility, Visibility, false, false, true)
REGISTER_ATTRIBUTE(Method, stat, Static, false, false, true)

QString Method::referenceName() const
{
	return name();
}

}
