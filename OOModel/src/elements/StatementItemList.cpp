/***********************************************************************************************************************
 * StatementItemList.cpp
 *
 *  Created on: Mar 23, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/StatementItemList.h"

namespace OOModel {

NODE_DEFINE_EMPTY_CONSTRUCTORS(StatementItemList, Model::TypedList<StatementItem> )
NODE_DEFINE_TYPE_REGISTRATION_METHODS(StatementItemList)

Model::Node* StatementItemList::navigateTo(Model::Node* source, QString path)
{
	if (isAncestorOf(source))
	{
		QString symbol = extractFrontSymbol(path);
		Node* found = NULL;

		// We should look for a statement above the source that declares a symbol.
		for(int i = indexOf(source); i >= 0; --i)
		{
			if ( at(i)->definesSymbol() && at(i)->symbolName() == symbol)
			{
				found = at(i);
				break;
			}
		}

		if (!found ) return Model::TypedList<StatementItem>::navigateTo(source, path);

		QString rest = extractSecondaryPath(path);
		if (!rest.isEmpty()) return found->navigateTo(this, rest);
		else return found;
	}
	else return NULL;

}

}
