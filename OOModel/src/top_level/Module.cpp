/***********************************************************************************************************************
 * Module.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/Module.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Module, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Module, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Module, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Module, modules, TypedListOfModule, false, false, true)
REGISTER_ATTRIBUTE(Module, classes, TypedListOfClass, false, false, true)

bool Module::definesSymbol() const
{
	return true;
}

const QString& Module::symbolName() const
{
	return name();
}

Model::Node* Module::navigateTo(Model::Node* source, QString path)
{
	QString symbol = extractFrontSymbol(path);
	Model::Node* found = NULL;

	// Is the target symbol name the module's name
	if (isAncestorOf(source) && symbol == symbolName()) found = this;

	if (!found) found = modules()->findFirstSymbolDefinition(symbol);
	if (!found) found = classes()->findFirstSymbolDefinition(symbol);
	if (!found) return ExtendableNode::navigateTo(source, path);

	QString rest = extractSecondaryPath(path);
	if (!rest.isEmpty()) return found->navigateTo(this, rest);
	else return found;
}

}
