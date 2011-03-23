/***********************************************************************************************************************
 * Project.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/Project.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Project, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Project, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Project, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Project, projects, TypedListOfProject, false, false, true)
REGISTER_ATTRIBUTE(Project, libraries, TypedListOfLibrary, false, false, true)
REGISTER_ATTRIBUTE(Project, modules, TypedListOfModule, false, false, true)
REGISTER_ATTRIBUTE(Project, classes, TypedListOfClass, false, false, true)

bool Project::definesSymbol() const
{
	return true;
}

const QString& Project::symbolName() const
{
	return name();
}

Model::Node* Project::navigateTo(Model::Node* source, QString path)
{
	QString symbol = extractFrontSymbol(path);
	Model::Node* found = NULL;

	// Is the target symbol name the module's name
	if (isAncestorOf(source) && symbol == symbolName()) found = this;

	if (!found) found = projects()->findFirstSymbolDefinition(symbol);
	if (!found) found = libraries()->findFirstSymbolDefinition(symbol);
	if (!found) found = modules()->findFirstSymbolDefinition(symbol);
	if (!found) found = classes()->findFirstSymbolDefinition(symbol);
	if (!found) return ExtendableNode::navigateTo(source, path);

	QString rest = extractSecondaryPath(path);
	if (!rest.isEmpty()) return found->navigateTo(this, rest);
	else return found;
}

}
