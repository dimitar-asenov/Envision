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
REGISTER_ATTRIBUTE(Method, storageSpecifier, StorageSpecifier, false, false, true)

bool Method::definesSymbol() const
{
	return true;
}

const QString& Method::symbolName() const
{
	return name();
}

Model::Node* Method::navigateTo(Model::Node* source, QString path)
{
	if (isAncestorOf(source))
	{
		QString symbol = extractFrontSymbol(path);
		Model::Node* found = NULL;

		// Is the target symbol name the method's name
		if (symbol == symbolName()) found = this;

		if ( !found && source == items() )
		{
			if (!found) found = arguments()->findFirstSymbolDefinition(symbol);
			if (!found) found = results()->findFirstSymbolDefinition(symbol);
		}

		if (!found) return ExtendableNode::navigateTo(source, path);

		QString rest = extractSecondaryPath(path);
		if (!rest.isEmpty()) return found->navigateTo(this, rest);
		else return found;
	}
	else return NULL;
}

}
