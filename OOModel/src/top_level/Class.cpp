/***********************************************************************************************************************
 * Class.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/Class.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Class, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Class, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Class, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Class, baseClasses, TypedListOfType, false, false, true)
REGISTER_ATTRIBUTE(Class, fields, TypedListOfField, false, false, true)
REGISTER_ATTRIBUTE(Class, methods, TypedListOfMethod, false, false, true)
REGISTER_ATTRIBUTE(Class, visibility, Visibility, false, false, true)

bool Class::definesSymbol() const
{
	return true;
}

const QString& Class::symbolName() const
{
	return name();
}

Model::Node* Class::navigateTo(Model::Node* source, QString path)
{
	QString symbol = extractFrontSymbol(path);
	Model::Node* found = NULL;

	// Is the target symbol name the class' name
	if (isAncestorOf(source) && symbol == symbolName()) found = this;

	if (!found) found = methods()->findFirstSymbolDefinition(symbol);
	if (!found) found = fields()->findFirstSymbolDefinition(symbol);
	if (!found) return ExtendableNode::navigateTo(source, path);

	QString rest = extractSecondaryPath(path);
	if (!rest.isEmpty()) return found->navigateTo(this, rest);
	else return found;
}

}
